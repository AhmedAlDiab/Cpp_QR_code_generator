#pragma once

#include <iostream>
#include <string>
#include <wx/filename.h>
#include <wx/datetime.h>
#include <wx/image.h> 
#include <wx/stdpaths.h>
#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/statbmp.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/uri.h>
#include "qrcodegen.hpp"
#include <wx/simplebook.h>
#include <wx/checkbox.h>

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    enum class PayloadType { PlainUrl, VCard, Wifi, Mail, SMS };

    // Common UI
    wxStaticBitmap* QrView{};
    wxStaticText* SaveInfo{};
    wxChoice* Option{};
    wxGauge* Progress{};
    wxSimplebook* Book{};
    wxButton* Submit{};

    // Pages
    wxPanel* PagePlain{};
    wxPanel* PageVCard{};
    wxPanel* PageWifi{};
    wxPanel* PageMail{};
    wxPanel* PageSMS{};

    // Plain
    wxTextCtrl* PlainText{};

    // vCard
    wxTextCtrl* V_FN{};
    wxTextCtrl* V_TEL{};
    wxTextCtrl* V_EMAIL{};
    wxTextCtrl* V_ORG{};
    wxTextCtrl* V_TITLE{};
    wxTextCtrl* V_URL{};

    // WiFi
    wxTextCtrl* W_SSID{};
    wxTextCtrl* W_PASS{};
    wxChoice* W_SEC{};
    wxCheckBox* W_HIDDEN{};

    // Mail
    wxTextCtrl* M_TO{};
    wxTextCtrl* M_SUBJ{};
    wxTextCtrl* M_BODY{};

    // SMS
    wxTextCtrl* S_TO{};
    wxTextCtrl* S_MSG{};

    // Build UI pages
    wxPanel* MakePlainPage(wxWindow* parent);
    wxPanel* MakeVCardPage(wxWindow* parent);
    wxPanel* MakeWifiPage(wxWindow* parent);
    wxPanel* MakeMailPage(wxWindow* parent);
    wxPanel* MakeSMSPage(wxWindow* parent);

    // Logic
    PayloadType GetSelectedType() const;
    wxString BuildPayload() const;

    // Events
    void OnTypeChanged(wxCommandEvent& event);
    void GenerateQRCode(wxCommandEvent& event);
};

class QR_App : public wxApp
{
public:
    bool OnInit() override;
};


