#ifndef __FBSEARCHCOMBO_H__
#define __FBSEARCHCOMBO_H__

#include <wx/wx.h>
#include <wx/combo.h>

class FbSearchCombo: public wxComboCtrl
{
	public:
		FbSearchCombo() : wxComboCtrl() { Init(); }

		FbSearchCombo(wxWindow *parent,
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

			SetButtonBitmaps(RenderButtonBitmap(), true);
		}

		virtual void OnButtonClick()
		{
			wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED,  GetId());
			wxPostEvent(this, event);
		}

		virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {}

	private:
		// Initialize member variables here
		void Init() {}
	private:
		wxBitmap RenderButtonBitmap();
};

#endif // __FBSEARCHCOMBO_H__
