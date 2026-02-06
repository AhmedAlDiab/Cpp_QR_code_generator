#include "QR_code_generator.h"

using qrcodegen::QrCode;

wxIMPLEMENT_APP(QR_App);

bool QR_App::OnInit()
{
    wxInitAllImageHandlers();

    MyFrame* frame = new MyFrame("QR Code Generator");
    frame->SetClientSize(wxSize(1024, 720));
    frame->Center();
    frame->Show(true);
    return true;
}

static const wxArrayString Options = {
    "Plain Text/URL",
    "Vcard",
    "Wifi",
    "Mail",
    "SMS"
};

// label + control row
static wxSizer* Row(wxWindow* parent, const wxString& label, wxWindow* ctrl)
{
    auto* s = new wxBoxSizer(wxHORIZONTAL);
    s->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    s->Add(ctrl, 1, wxEXPAND);
    return s;
}

// Escape for WIFI: (backslash + ; , : " are special)
static wxString EscapeWifi(wxString s)
{
    s.Replace("\\", "\\\\");
    s.Replace(";", "\\;");
    s.Replace(",", "\\,");
    s.Replace(":", "\\:");
    s.Replace("\"", "\\\"");
    return s;
}

// URL-encode (for mailto query params). Works on all wx versions.
static wxString UrlEncode(const wxString& s)
{
    wxString out;
    out.reserve(s.size() * 3);

    for (wxUniChar ch : s) {
        unsigned int c = (unsigned int)ch.GetValue();

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out += (wxChar)c;
        }
        else if (c == ' ') {
            out += "%20";
        }
        else {
            out += wxString::Format("%%%02X", c & 0xFF);
        }
    }
    return out;
}

// ---------------- Pages ----------------

wxPanel* MyFrame::MakePlainPage(wxWindow* parent)
{
    auto* p = new wxPanel(parent);

    PlainText = new wxTextCtrl(p, wxID_ANY, "");
    PlainText->SetHint("Text or URL (https://...)");

    auto* s = new wxBoxSizer(wxVERTICAL);
    s->Add(Row(p, "Text/URL:", PlainText), 0, wxEXPAND | wxALL, 8);
    s->AddStretchSpacer(1);
    p->SetSizer(s);
    return p;
}

wxPanel* MyFrame::MakeVCardPage(wxWindow* parent)
{
    auto* p = new wxPanel(parent);

    V_FN = new wxTextCtrl(p, wxID_ANY, "");
    V_TEL = new wxTextCtrl(p, wxID_ANY, "");
    V_EMAIL = new wxTextCtrl(p, wxID_ANY, "");
    V_ORG = new wxTextCtrl(p, wxID_ANY, "");
    V_TITLE = new wxTextCtrl(p, wxID_ANY, "");
    V_URL = new wxTextCtrl(p, wxID_ANY, "");

    auto* s = new wxBoxSizer(wxVERTICAL);
    s->Add(Row(p, "Full name:", V_FN), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Phone:", V_TEL), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Email:", V_EMAIL), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Org:", V_ORG), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Title:", V_TITLE), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Website:", V_URL), 0, wxEXPAND | wxALL, 6);
    s->AddStretchSpacer(1);

    p->SetSizer(s);
    return p;
}

wxPanel* MyFrame::MakeWifiPage(wxWindow* parent)
{
    auto* p = new wxPanel(parent);

    W_SSID = new wxTextCtrl(p, wxID_ANY, "");
    W_PASS = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

    wxArrayString sec = { "WPA", "WEP", "nopass" };
    W_SEC = new wxChoice(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, sec);
    W_SEC->SetSelection(0);

    W_HIDDEN = new wxCheckBox(p, wxID_ANY, "Hidden SSID");

    // Disable password when "nopass"
    W_SEC->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) {
        bool needsPass = (W_SEC->GetStringSelection() != "nopass");
        W_PASS->Enable(needsPass);
        if (!needsPass) W_PASS->SetValue("");
        });

    auto* s = new wxBoxSizer(wxVERTICAL);
    s->Add(Row(p, "SSID:", W_SSID), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Password:", W_PASS), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Security:", W_SEC), 0, wxEXPAND | wxALL, 6);
    s->Add(W_HIDDEN, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    s->AddStretchSpacer(1);

    p->SetSizer(s);
    return p;
}

wxPanel* MyFrame::MakeMailPage(wxWindow* parent)
{
    auto* p = new wxPanel(parent);

    M_TO = new wxTextCtrl(p, wxID_ANY, "");
    M_SUBJ = new wxTextCtrl(p, wxID_ANY, "");
    M_BODY = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 110), wxTE_MULTILINE);

    auto* s = new wxBoxSizer(wxVERTICAL);
    s->Add(Row(p, "To:", M_TO), 0, wxEXPAND | wxALL, 6);
    s->Add(Row(p, "Subject:", M_SUBJ), 0, wxEXPAND | wxALL, 6);

    // Body row (label + multiline)
    auto* bodyRow = new wxBoxSizer(wxHORIZONTAL);
    bodyRow->Add(new wxStaticText(p, wxID_ANY, "Body:"), 0, wxALIGN_TOP | wxRIGHT, 8);
    bodyRow->Add(M_BODY, 1, wxEXPAND);
    s->Add(bodyRow, 1, wxEXPAND | wxALL, 6);

    p->SetSizer(s);
    return p;
}

wxPanel* MyFrame::MakeSMSPage(wxWindow* parent)
{
    auto* p = new wxPanel(parent);

    S_TO = new wxTextCtrl(p, wxID_ANY, "");
    S_MSG = new wxTextCtrl(p, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 110), wxTE_MULTILINE);

    auto* s = new wxBoxSizer(wxVERTICAL);
    s->Add(Row(p, "To:", S_TO), 0, wxEXPAND | wxALL, 6);

    auto* msgRow = new wxBoxSizer(wxHORIZONTAL);
    msgRow->Add(new wxStaticText(p, wxID_ANY, "Message:"), 0, wxALIGN_TOP | wxRIGHT, 8);
    msgRow->Add(S_MSG, 1, wxEXPAND);
    s->Add(msgRow, 1, wxEXPAND | wxALL, 6);

    p->SetSizer(s);
    return p;
}

// ---------------- Logic ----------------

MyFrame::PayloadType MyFrame::GetSelectedType() const
{
    switch (Option->GetSelection()) {
    case 0: return PayloadType::PlainUrl;
    case 1: return PayloadType::VCard;
    case 2: return PayloadType::Wifi;
    case 3: return PayloadType::Mail;
    case 4: return PayloadType::SMS;
    default: return PayloadType::PlainUrl;
    }
}

wxString MyFrame::BuildPayload() const
{
    switch (GetSelectedType()) {
    case PayloadType::PlainUrl:
        return PlainText->GetValue();

    case PayloadType::VCard: {
        wxString v =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n";

        if (!V_FN->GetValue().empty())    v += "FN:" + V_FN->GetValue() + "\n";
        if (!V_TEL->GetValue().empty())   v += "TEL:" + V_TEL->GetValue() + "\n";
        if (!V_EMAIL->GetValue().empty()) v += "EMAIL:" + V_EMAIL->GetValue() + "\n";
        if (!V_ORG->GetValue().empty())   v += "ORG:" + V_ORG->GetValue() + "\n";
        if (!V_TITLE->GetValue().empty()) v += "TITLE:" + V_TITLE->GetValue() + "\n";
        if (!V_URL->GetValue().empty())   v += "URL:" + V_URL->GetValue() + "\n";

        v += "END:VCARD";
        return v;
    }

    case PayloadType::Wifi: {
        wxString sec = W_SEC->GetStringSelection(); // WPA/WEP/nopass
        wxString ssid = EscapeWifi(W_SSID->GetValue());
        wxString pass = EscapeWifi(W_PASS->GetValue());

        wxString w = "WIFI:T:" + sec + ";S:" + ssid + ";";
        if (sec != "nopass")
            w += "P:" + pass + ";";
        if (W_HIDDEN && W_HIDDEN->GetValue())
            w += "H:true;";
        w += ";";
        return w;
    }

    case PayloadType::Mail: {
        wxString to = M_TO->GetValue();
        wxString subj = UrlEncode(M_SUBJ->GetValue());
        wxString body = UrlEncode(M_BODY->GetValue());

        wxString m = "mailto:" + to;
        bool hasQ = false;
        if (!subj.empty()) { m += "?subject=" + subj; hasQ = true; }
        if (!body.empty()) { m += (hasQ ? "&" : "?") + wxString("body=") + body; }
        return m;
    }

    case PayloadType::SMS:
        return "SMSTO:" + S_TO->GetValue() + ":" + S_MSG->GetValue();
    }

    return "";
}

// ---------------- Events ----------------

void MyFrame::OnTypeChanged(wxCommandEvent&)
{
    // Switch the book page to match the dropdown selection
    Book->ChangeSelection(Option->GetSelection());
    Book->Layout();
}

void MyFrame::GenerateQRCode(wxCommandEvent&)
{
    Progress->SetValue(10);

    wxString payloadWx = BuildPayload();
    std::string payload = payloadWx.ToUTF8().data();

    if (payload.empty()) {
        wxMessageBox("Payload is empty.", "Error", wxOK | wxICON_WARNING);
        Progress->SetValue(0);
        return;
    }

    Progress->SetValue(30);

    QrCode qr = QrCode::encodeText(payload.c_str(), QrCode::Ecc::MEDIUM);

    int size = qr.getSize();
    int target = 320;
    int scale = std::max(4, target / size);

    wxBitmap bmp(size * scale, size * scale);
    {
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (qr.getModule(x, y)) {
                    dc.DrawRectangle(x * scale, y * scale, scale, scale);
                }
            }
        }
    }

    Progress->SetValue(70);

    QrView->SetBitmap(bmp);
    QrView->Refresh();

    // Save unique file
    wxString folder = wxGetCwd() + "\\qr_out";
    if (!wxFileName::DirExists(folder))
        wxFileName::Mkdir(folder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    wxDateTime now = wxDateTime::UNow();
    wxString name = now.Format("qr_%Y%m%d_%H%M%S_") + wxString::Format("%03d", now.GetMillisecond());
    wxString path = folder + "\\" + name + ".png";

    const int WRAP_W = 320; // keep consistent with preview width

    if (!bmp.SaveFile(path, wxBITMAP_TYPE_PNG)) {
        SaveInfo->SetLabel("❌ Failed to save file");
        SaveInfo->SetForegroundColour(*wxRED);

        // IMPORTANT: prevent wxStaticText from auto-growing horizontally
        SaveInfo->SetMinSize(wxSize(WRAP_W, -1));
        SaveInfo->SetMaxSize(wxSize(WRAP_W, -1));
        SaveInfo->Wrap(WRAP_W);

        // Layout the main panel so the sizer recalculates the height
        if (SaveInfo->GetParent()) SaveInfo->GetParent()->Layout();
        if (GetParent()) GetParent()->Layout();
        Layout();

        return;
    }

    SaveInfo->SetLabel("Saved as:\n" + path);
    SaveInfo->SetForegroundColour(*wxGREEN);

    // Keep it wrapped and relayout so height recalculates properly
    SaveInfo->SetMinSize(wxSize(WRAP_W, -1));
    SaveInfo->SetMaxSize(wxSize(WRAP_W, -1));
    SaveInfo->Wrap(WRAP_W);

    if (SaveInfo->GetParent()) SaveInfo->GetParent()->Layout();
    if (GetParent()) GetParent()->Layout();
    Layout();

    Progress->SetValue(100);
}

// ---------------- Frame ctor ----------------

MyFrame::MyFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
    wxPanel* Main = new wxPanel(this);

    Option = new wxChoice(Main, wxID_ANY, wxDefaultPosition, wxDefaultSize, Options);
    Option->SetSelection(0);

    Submit = new wxButton(Main, wxID_ANY, "Generate");
    Progress = new wxGauge(Main, wxID_ANY, 100);
    Progress->SetValue(0);

    QrView = new wxStaticBitmap(Main, wxID_ANY, wxNullBitmap);
    QrView->SetMinSize(wxSize(320, 320));

    // Book: cleanly switches pages (fixes your "only 1 field visible" issue)
    Book = new wxSimplebook(Main, wxID_ANY);

    PagePlain = MakePlainPage(Book);
    PageVCard = MakeVCardPage(Book);
    PageWifi = MakeWifiPage(Book);
    PageMail = MakeMailPage(Book);
    PageSMS = MakeSMSPage(Book);

    Book->AddPage(PagePlain, "Plain", true);
    Book->AddPage(PageVCard, "VCard", false);
    Book->AddPage(PageWifi, "WiFi", false);
    Book->AddPage(PageMail, "Mail", false);
    Book->AddPage(PageSMS, "SMS", false);

    // Layout
    auto* root = new wxBoxSizer(wxHORIZONTAL);

    auto* previewBox = new wxStaticBoxSizer(wxVERTICAL, Main, "Preview");

    // Keep QR at its min size, centered
    previewBox->Add(QrView, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 12);

    // ---- WRAP FIX:
    // wxStaticText auto-resizes by default (can grow wide, breaking Wrap()).
    // Use wxST_NO_AUTORESIZE and clamp width with min/max size.
    const int PREVIEW_W = 320;

    SaveInfo = new wxStaticText(
        Main,
        wxID_ANY,
        "No QR generated yet",
        wxDefaultPosition,
        wxDefaultSize,
        wxST_NO_AUTORESIZE | wxALIGN_CENTER_HORIZONTAL
    );
    SaveInfo->SetForegroundColour(*wxLIGHT_GREY);

    // Clamp width so it can't expand horizontally (so Wrap actually wraps)
    SaveInfo->SetMinSize(wxSize(PREVIEW_W, -1));
    SaveInfo->SetMaxSize(wxSize(PREVIEW_W, -1));

    // Wrap to the preview width
    SaveInfo->Wrap(PREVIEW_W);

    // Add it centered (or use EXPAND; width is clamped anyway)
    previewBox->Add(SaveInfo, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_HORIZONTAL, 10);

    auto* controlsBox = new wxStaticBoxSizer(wxVERTICAL, Main, "Controls");

    auto* grid = new wxFlexGridSizer(2, 10, 10);
    grid->AddGrowableCol(1, 1);
    grid->AddGrowableRow(1, 1); // gives the page area height

    grid->Add(new wxStaticText(Main, wxID_ANY, "Type:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(Option, 1, wxEXPAND);

    grid->Add(new wxStaticText(Main, wxID_ANY, "Fields:"), 0, wxALIGN_TOP);
    grid->Add(Book, 1, wxEXPAND);

    grid->Add(new wxStaticText(Main, wxID_ANY, "Progress:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(Progress, 1, wxEXPAND);

    controlsBox->Add(grid, 1, wxALL | wxEXPAND, 12);

    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->AddStretchSpacer(1);
    btnRow->Add(Submit, 0);
    controlsBox->Add(btnRow, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 12);

    root->Add(previewBox, 1, wxALL | wxEXPAND, 12);
    root->Add(controlsBox, 1, wxALL | wxEXPAND, 12);

    Main->SetSizer(root);
    Main->Layout();

    // Events
    Submit->Bind(wxEVT_BUTTON, &MyFrame::GenerateQRCode, this);
    Option->Bind(wxEVT_CHOICE, &MyFrame::OnTypeChanged, this);

    SetClientSize(1000, 650);
    Center();
}
