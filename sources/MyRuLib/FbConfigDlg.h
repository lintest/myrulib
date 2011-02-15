#ifndef __FBCONFIGDLG_H__
#define __FBCONFIGDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/arrimpl.cpp>
#include "FbBookEvent.h"
#include "controls/FbTreeModel.h"
#include "FbWindow.h"
#include "FbDatabase.h"

///////////////////////////////////////////////////////////////////////////////
/// Class FbConfigDlg
///////////////////////////////////////////////////////////////////////////////
class FbConfigDlg : private FbDialog
{
	public:
		FbConfigDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER);
		virtual ~FbConfigDlg();
		static void Execute(wxWindow* parent);
	private:
		enum ID {
			ID_LIBRARY_TITLE = 1000,
			ID_LIBRARY_DIR,
			ID_LIBRARY_DESCR,
			ID_DOWNLOAD_HOST,
			ID_DOWNLOAD_ADDR,
			ID_DOWNLOAD_USER,
			ID_DOWNLOAD_PASS,
			ID_TYPE_LIST,
			ID_TYPE_TOOLBAR,
			ID_APPEND_TYPE,
			ID_MODIFY_TYPE,
			ID_DELETE_TYPE,
		};
		class PanelMain: public wxPanel
		{
			public:
				PanelMain(wxWindow *parent);
		};
		class PanelInternet: public wxPanel
		{
			public:
				PanelInternet(wxWindow *parent);
		};
		class PanelTypes: public wxPanel
		{
			public:
				PanelTypes(wxWindow *parent);
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
				wxEvtHandler * m_frame;
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
	private:
		void SaveTypes(wxSQLite3Database &database);
		void EnableTool(wxWindowID id, bool enable);
		void Assign(bool write);
	private:
		FbCommonDatabase m_database;
		LoadThread m_thread;
		wxArrayString m_del_type;
	private:
		void OnSelectFolderClick( wxCommandEvent& event );
		void OnAppendType( wxCommandEvent& event );
		void OnModifyType( wxCommandEvent& event );
		void OnDeleteType( wxCommandEvent& event );
		void OnTypeActivated( wxTreeEvent & event );
		void OnModel( FbModelEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBCONFIGDLG_H__
