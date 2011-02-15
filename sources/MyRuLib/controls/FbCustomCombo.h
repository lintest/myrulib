#ifndef __FBCUSTOMCOMBO_H__
#define __FBCUSTOMCOMBO_H__

#include <wx/combo.h>

class FbCustomCombo : public wxComboCtrl
{
public:
    FbCustomCombo() : wxComboCtrl() { Init(); }

    FbCustomCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        //
        // Prepare custom button bitmap (just '...' text)
        wxMemoryDC dc;
        wxBitmap tmp;
        dc.SelectObject(tmp);

        wxString str = wxChar(0x2026);
        wxSize s = dc.GetTextExtent(str);
        wxBitmap bmp(s.x + 2, s.y + 2);
        dc.SelectObject(bmp);

        // Draw transparent background
        wxColour colour(255, 0, 255);
        wxBrush brush(colour);
        dc.SetBrush(brush);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
        dc.DrawText(str, 0, 0);

        dc.SelectObject(wxNullBitmap);

        // Finalize transparency with a mask
        wxMask *mask = new wxMask(bmp, colour);
        bmp.SetMask(mask);

        SetButtonBitmaps(bmp, true);
    }

    virtual void OnButtonClick()
    {
    	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED,  GetId());
    	wxPostEvent(this, event);
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup))
    {
    }

private:
    // Initialize member variables here
    void Init()
    {
    }
};

#endif // __FBCUSTOMCOMBO_H__
