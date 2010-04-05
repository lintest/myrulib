#include "FbSearchCtrl.h"

BEGIN_EVENT_TABLE( FbSearchCtrl, wxSearchCtrl )
    EVT_CHAR(FbSearchCtrl::OnChar)
END_EVENT_TABLE()

FbSearchCtrl::FbSearchCtrl(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    : wxSearchCtrl(parent, id, value, pos, size, style, validator, name)
{
}

bool FbSearchCtrl::Create(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    return wxSearchCtrl::Create(parent, id, value, pos, size, style, validator, name);
}

void FbSearchCtrl::OnChar(wxKeyEvent &event)
{
    switch (event.GetKeyCode()) {
        case WXK_RETURN: {
            wxLogMessage(wxT("DataViewFrame::OnChar(WXK_RETURN)"));
        } break;
        case WXK_TAB: {
            Navigate();
        } break;
        default: {
            event.Skip();
        }
    }
}
