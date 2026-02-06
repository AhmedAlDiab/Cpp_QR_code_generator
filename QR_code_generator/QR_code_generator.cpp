#include "QR_code_generator.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "wxWidgets Test",
        wxDefaultPosition, wxSize(400, 300))
{
    new wxStaticText(this, wxID_ANY, "wxWidgets is working!",
        wxPoint(20, 20));
}
