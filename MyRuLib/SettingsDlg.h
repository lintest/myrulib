///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SettingsDlg__
#define __SettingsDlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl3;
		wxBitmapButton* m_bpButton2;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl2;
		wxBitmapButton* m_bpButton1;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl4;
		wxBitmapButton* m_bpButton3;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrl6;
		wxBitmapButton* m_bpButton5;
		wxCheckBox* m_checkBox1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText51;
		wxTextCtrl* m_textCtrl51;
		wxBitmapButton* m_bpButton41;
		wxCheckBox* m_checkBox11;
		wxCheckBox* m_checkBox111;
		wxRadioBox* m_radioBox2;
		wxRadioBox* m_radioBox21;
		wxCheckBox* m_checkBox1111;
		wxPanel* m_panel3;
		wxCheckBox* m_checkBox2;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl7;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textCtrl8;
		wxStdDialogButtonSizer* m_sdbSizer3;
		wxButton* m_sdbSizer3OK;
		wxButton* m_sdbSizer3Cancel;
	
	public:
		SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SettingsDlg();
	
};

#endif //__SettingsDlg__
