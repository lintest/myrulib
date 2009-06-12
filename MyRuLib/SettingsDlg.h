///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SettingsDlg__
#define __SettingsDlg__

#include <wx/wx.h>
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

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : public wxDialog
{
	DECLARE_EVENT_TABLE()
	private:
		enum
		{
			IDS_LIBRARY_TITLE = 1000,
			IDS_PROGRAM_TXT,
			IDS_PROGRAM_BTN,
			IDS_LIBRARY_DIR_TXT,
			IDS_LIBRARY_DIR_BTN,
			IDS_INTERNET_DIR_TXT,
			IDS_INTERNET_DIR_BTN,
			IDS_TEMPORARY_DIR_TXT,
			IDS_TEMPORARY_DIR_BTN,
			IDS_TEMPORARY_DEL,
			IDS_EXTERNAL_TXT,
			IDS_EXTERNAL_BTN,
			IDS_TRANSLIT_FOLDER,
			IDS_TRANSLIT_FILE,
			IDS_FOLDER_FORMAT,
			IDS_FILE_FORMAT,
			IDS_USE_PROXY,
			IDS_PROXY_ADDR,
			IDS_PROXY_PORT,
			IDS_PROXY_NAME,
			IDS_PROXY_PASS,
		};
		void OnSelectFileClick( wxCommandEvent& event );
		void OnSelectFolderClick( wxCommandEvent& event );
	private:
		wxNotebook* m_notebook;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl5;
		wxBitmapButton* m_bpButton5;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl2;
		wxBitmapButton* m_bpButton2;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrl3;
		wxBitmapButton* m_bpButton3;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl4;
		wxBitmapButton* m_bpButton4;
		wxCheckBox* m_checkBox1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrl6;
		wxBitmapButton* m_bpButton6;
		wxCheckBox* m_checkBox2;
		wxCheckBox* m_checkBox3;
		wxRadioBox* m_radioBox1;
		wxRadioBox* m_radioBox2;
		wxPanel* m_panel3;
		wxCheckBox* m_checkBox21;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrl7;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textCtrl8;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl9;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrl10;
		wxStdDialogButtonSizer* m_sdbSizerBtn;
		wxButton* m_sdbSizerBtnOK;
		wxButton* m_sdbSizerBtnCancel;

	public:
		SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SettingsDlg();

};

#endif //__SettingsDlg__
