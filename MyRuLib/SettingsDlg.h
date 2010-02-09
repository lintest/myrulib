///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SettingsDlg__
#define __SettingsDlg__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/arrimpl.cpp>
#include <wx/fontpicker.h>
#include <wx/spinctrl.h>
#include "FbDatabase.h"
#include "FbWindow.h"

///////////////////////////////////////////////////////////////////////////////
/// Class SettingsDlg
///////////////////////////////////////////////////////////////////////////////
class SettingsDlg : private FbDialog
{
	private:
		enum ID {
			ID_AUTO_DOWNLD = 1000,
			ID_TEMP_DEL,
			ID_TEMP_DIR_TXT,
			ID_TEMP_DIR_BTN,
			ID_DOWNLOAD_DIR_TXT,
			ID_DOWNLOAD_DIR_BTN,
			ID_USE_PROXY,
			ID_PROXY_ADDR,
			ID_HTTP_IMAGES,
			ID_LIBRUSEC_URL,
			ID_EXTERNAL_TXT,
			ID_EXTERNAL_BTN,
			ID_DEL_DOWNLOAD,
			ID_TRANSLIT_FOLDER,
			ID_TRANSLIT_FILE,
			ID_FOLDER_FORMAT,
			ID_FILE_FORMAT,
			ID_USE_SYMLINKS,
			ID_TYPELIST,
			ID_APPEND_TYPE,
			ID_MODIFY_TYPE,
			ID_DELETE_TYPE,
			ID_FONT_CLEAR,
			ID_FONT_MAIN,
			ID_FONT_HTML,
			ID_FONT_TOOL,
			ID_FONT_DLG,
			ID_REMOVE_FILES,
			ID_AUTOHIDE_COLUMN,
			ID_LIMIT_CHECK,
			ID_LIMIT_COUNT,
			ID_SAVE_FULLPATH,
		};
		class FbPanelFont: public wxPanel
		{
			public:
				FbPanelFont(wxWindow *parent);
			private:
				void AppendItem(wxFlexGridSizer* fgSizer, const wxString& name, wxWindowID winid = wxID_ANY);
		};
		class FbPanelInternet: public wxPanel
		{
			public:
				FbPanelInternet(wxWindow *parent);
		};
		class FbPanelExport: public wxPanel
		{
			public:
				FbPanelExport(wxWindow *parent);
		};
		class FbPanelTypes: public wxPanel
		{
			public:
				FbPanelTypes(wxWindow *parent);
		};
		class FbPanelInterface: public wxPanel
		{
			public:
				FbPanelInterface(wxWindow *parent);
		};
	public:
		SettingsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		virtual ~SettingsDlg();
		static void Execute(wxWindow* parent);
	private:
		void Assign(bool write);
		void SetFont( wxWindowID id, wxFont font );
		void SelectApplication();
		void FillTypelist();
		void SaveTypelist();
	private:
		void OnSelectFolderClick( wxCommandEvent& event );
		void OnAppendType( wxCommandEvent& event );
		void OnModifyType( wxCommandEvent& event );
		void OnDeleteType( wxCommandEvent& event );
		void OnTypelistActivated( wxListEvent & event );
		void OnFontClear( wxCommandEvent& event );
	private:
		wxArrayString m_commands;
		wxArrayString m_deleted;
		DECLARE_EVENT_TABLE()
};

#endif //__SettingsDlg__
