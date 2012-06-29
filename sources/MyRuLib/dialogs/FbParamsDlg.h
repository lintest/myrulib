#ifndef __FBPARAMSDLG_H__
#define __FBPARAMSDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/treebase.h>
#include <wx/wxsqlite3.h>
#include "FbWindow.h"
#include "controls/FbTreeModel.h"

class FbTreeViewCtrl;

///////////////////////////////////////////////////////////////////////////////
/// Class FbParamsDlg
///////////////////////////////////////////////////////////////////////////////
class FbParamsDlg : private FbDialog
{
	private:
		enum ID {
			ID_AUTO_DOWNLD = 1000,
			ID_TEMP_DEL,
			ID_TEMP_DIR,
			ID_WINE_DIR,
			ID_DOWNLOAD_DIR,
			ID_USE_PROXY,
			ID_PROXY_ADDR,
			ID_PROXY_USER,
			ID_PROXY_PASS,
			ID_HTTP_IMAGES,
			ID_LIBRUSEC_URL,
			ID_EXTERNAL_DIR,
			ID_DEL_DOWNLOAD,
			ID_EXAMPLE,
			ID_TRANSLIT_FOLDER,
			ID_TRANSLIT_FILE,
			ID_USE_UNDERSCORE,
			ID_FOLDER_FORMAT,
			ID_NUMBER_FORMAT,
			ID_FILE_FORMAT,
			ID_USE_SYMLINKS,
			ID_TYPE_LIST,
			ID_TYPE_TOOLBAR,
			ID_USE_COOLREADER,
			ID_APPEND_TYPE,
			ID_MODIFY_TYPE,
			ID_DELETE_TYPE,
			ID_SCRIPT_LIST,
			ID_SCRIPT_TOOLBAR,
			ID_APPEND_SCRIPT,
			ID_MODIFY_SCRIPT,
			ID_DELETE_SCRIPT,
			ID_FONT_CLEAR,
			ID_FONT_MAIN,
			ID_FONT_HTML,
			ID_FONT_TOOL,
			ID_FONT_DLG,
			ID_COLOUR_MAIN,
			ID_COLOUR_HTML,
			ID_COLOUR_TOOL,
			ID_COLOUR_DLG,
			ID_REMOVE_FILES,
			ID_CLEAR_LOG,
			ID_GRAY_FONT,
			ID_GRID_HRULES,
			ID_GRID_VRULES,
			ID_SAVE_FULLPATH,
			ID_LANG_LOCALE,
			ID_LETTERS,
			ID_WEB_TIMEOUT,
			ID_WEB_ATTEMPT,
			ID_FILE_LENGTH,
			ID_IMAGE_WIDTH,
		};
		class LoadThread: public FbThread
		{
			public:
				LoadThread(wxEvtHandler * frame)
					: FbThread(wxTHREAD_JOINABLE), m_frame(frame) {}
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
				void AppendItem(wxFlexGridSizer* fgSizer, const wxString& name, wxWindowID idFont, wxWindowID idColour);
		};
		class PanelInternet: public wxPanel
		{
			public:
				PanelInternet(wxWindow *parent);
		};
		class PanelExport: public wxPanel
		{
			public:
				PanelExport(wxWindow *parent, wxString &letters);
		};
		class PanelScripts: public wxPanel
		{
			public:
				PanelScripts(wxWindow *parent);
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
				TypeData(wxSQLite3ResultSet &result);
				TypeData(const wxString &type, const wxString &command = wxEmptyString)
					: m_type(type), m_command(command), m_modified(true) {}
			public:
				virtual wxString GetValue(FbModel & model, size_t col = 0) const;
				bool IsModified() { return m_modified; }
			protected:
				wxString m_type;
				wxString m_command;
				bool m_modified;
				DECLARE_CLASS(ScriptData);
		};
		class ScriptData: public FbModelData
		{
			public:
				ScriptData(wxSQLite3ResultSet &result);
				ScriptData(int code, const wxString &name, const wxString &text)
					: m_code(code), m_name(name), m_text(text), m_modified(true) {}
			public:
				virtual wxString GetValue(FbModel & model, size_t col = 0) const;
				int GetCode() { return m_code; }
				bool IsModified() { return m_modified; }
			protected:
				int m_code;
				wxString m_name;
				wxString m_text;
				bool m_modified;
				DECLARE_CLASS(ScriptData);
		};
		class ScriptDlg: public FbDialog
		{
			public:
				ScriptDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
				static bool Execute(wxWindow* parent, const wxString& title, wxString &name, wxString &text);
			protected:
				void OnLetterClicked(wxCommandEvent& event);
				wxTextCtrl m_name;
				wxTextCtrl m_text;
				wxString m_letters;
		};
	public:
		FbParamsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		virtual ~FbParamsDlg();
		static void Execute(wxWindow* parent);
	private:
		void Assign(bool write);
		void SetFont( wxWindowID id, wxFont font );
		void SetColour( wxWindowID id, wxColour colour );
		void SelectApplication();
		void SaveData();
		void DeleteTypes(wxSQLite3Database &database);
		void DeleteScripts(wxSQLite3Database &database);
		void SaveTypes(wxSQLite3Database &database);
		void SaveScripts(wxSQLite3Database &database);
		void SaveScripts();
		void EnableTool(wxWindowID id, bool enable);
		void FillFormats(FbTreeViewCtrl * treeview, FbModel * model);
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
		void OnLetterClicked(wxCommandEvent& event);
	private:
		wxString m_letters;
		wxArrayString m_del_type;
		wxArrayInt m_del_scr;
		LoadThread m_thread;
		DECLARE_EVENT_TABLE()
};

#endif // __FBPARAMSDLG_H__
