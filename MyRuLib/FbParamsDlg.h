#ifndef __FBPARAMSDLG_H__
#define __FBPARAMSDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/arrimpl.cpp>
#include <wx/fontpicker.h>
#include <wx/spinctrl.h>
#include <wx/wxsqlite3.h>
#include "FbWindow.h"
#include "FbTreeModel.h"

///////////////////////////////////////////////////////////////////////////////
/// Class FbParamsDlg
///////////////////////////////////////////////////////////////////////////////
class FbParamsDlg : private FbDialog
{
	private:
		enum ID {
			ID_AUTO_DOWNLD = 1000,
			ID_TEMP_DEL,
			ID_TEMP_DIR_TXT,
			ID_TEMP_DIR_BTN,
			ID_WINE_DIR,
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
			ID_TYPE_LIST,
			ID_APPEND_TYPE,
			ID_MODIFY_TYPE,
			ID_DELETE_TYPE,
			ID_APPEND_SCRIPT,
			ID_MODIFY_SCRIPT,
			ID_DELETE_SCRIPT,
			ID_SCRIPT_LIST,
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
		class LoadThread: public wxThread
		{
			public:
				LoadThread(wxEvtHandler * frame)
					:wxThread(wxTHREAD_JOINABLE), m_frame(frame) {}
			protected:
				virtual void * Entry();
			private:
				void LoadTypes(wxSQLite3Database &database);
				void LoadScripts(wxSQLite3Database &database);
				wxEvtHandler * m_frame;
		};
		class PanelFont: public wxPanel
		{
			public:
				PanelFont(wxWindow *parent);
			private:
				void AppendItem(wxFlexGridSizer* fgSizer, const wxString& name, wxWindowID winid = wxID_ANY);
		};
		class PanelInternet: public wxPanel
		{
			public:
				PanelInternet(wxWindow *parent);
		};
		class PanelExport: public wxPanel
		{
			public:
				PanelExport(wxWindow *parent);
		};
		class PanelTypes: public wxPanel
		{
			public:
				PanelTypes(wxWindow *parent);
		};
		class PanelInterface: public wxPanel
		{
			public:
				PanelInterface(wxWindow *parent);
		};
		class TypeData: public FbModelData
		{
			public:
				TypeData(const wxString &type, const wxString &command = wxEmptyString)
					: m_type(type), m_command(command) {}
			public:
				virtual wxString GetValue(FbModel & model, size_t col) const;
			protected:
				wxString m_type;
				wxString m_command;
				DECLARE_CLASS(ScriptData);
		};
		class ScriptData: public FbModelData
		{
			public:
				ScriptData(wxSQLite3ResultSet &result);
				ScriptData(int code, const wxString &name, const wxString &text)
					: m_code(code), m_name(name), m_text(text) {}
			public:
				virtual wxString GetValue(FbModel & model, size_t col) const;
				int GetCode() { return m_code; }
			protected:
				int m_code;
				wxString m_name;
				wxString m_text;
				DECLARE_CLASS(ScriptData);
		};
		class ScriptDlg: public FbDialog
		{
			public:
				ScriptDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
				static bool Execute(wxWindow* parent, const wxString& title, wxString &name, wxString &text);
			protected:
				wxTextCtrl m_name;
				wxTextCtrl m_text;
		};
	public:
		FbParamsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		virtual ~FbParamsDlg();
		static void Execute(wxWindow* parent);
	private:
		void Assign(bool write);
		void SetFont( wxWindowID id, wxFont font );
		void SelectApplication();
		void SaveTypelist();
	private:
        void OnClose( wxCloseEvent& event );
		void OnSelectFolderClick( wxCommandEvent& event );
		void OnAppendType( wxCommandEvent& event );
		void OnModifyType( wxCommandEvent& event );
		void OnDeleteType( wxCommandEvent& event );
		void OnAppendScript( wxCommandEvent& event );
		void OnModifyScript( wxCommandEvent& event );
		void OnDeleteScript( wxCommandEvent& event );
		void OnTypeActivated( wxTreeEvent & event );
		void OnScriptActivated( wxTreeEvent & event );
		void OnFontClear( wxCommandEvent& event );
		void OnModel( FbModelEvent& event );
	private:
		wxArrayString m_commands;
		wxArrayString m_deleted;
		wxArrayString m_scripts;
		wxArrayInt m_del_scr;
		LoadThread m_thread;
		DECLARE_EVENT_TABLE()
};

#endif // __FBPARAMSDLG_H__
