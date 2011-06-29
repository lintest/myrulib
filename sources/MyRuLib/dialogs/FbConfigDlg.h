#ifndef __FBCONFIGDLG_H__
#define __FBCONFIGDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/arrimpl.cpp>
#include <wx/gbsizer.h>
#include "FbBookEvent.h"
#include "controls/FbTreeModel.h"
#include "controls/FbTreeView.h"
#include "FbWindow.h"
#include "FbDatabase.h"

class FbDirectoryDlg: public FbDialog
{
public:
	FbDirectoryDlg( wxWindow * parent, const wxString& title ); 
private:
	enum ID {
		ID_TITLE = 1000,
		ID_DIR_FILE,
		ID_DIR_BASE,
		ID_DIR_CODE,
		ID_DIR_NAME,
		ID_DIR_KEYS,
		ID_REF_FILE,
		ID_REF_BASE,
		ID_REF_TYPE,
		ID_REF_CODE,
		ID_REF_BOOK,
	};
	void Append( int & row, wxWindowID id, const wxString & title );
	void Append( int & row, const wxString & title );
private:
	wxGridBagSizer * m_sizer;
};


///////////////////////////////////////////////////////////////////////////////
/// Class FbConfigDlg
///////////////////////////////////////////////////////////////////////////////
class FbConfigDlg : private FbDialog
{
public:
	static bool Execute(wxWindow * parent);
	FbConfigDlg(wxWindow * parent);

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
		ID_REFS_LIST,
		ID_TOOLBAR,
		ID_APPEND,
		ID_MODIFY,
		ID_DELETE,
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
			wxEvtHandler * m_frame;
	};
	
	class PanelTool: public wxPanel { 
		public: 
			PanelTool(wxWindow * parent); 
			virtual void Save(wxSQLite3Database &database) = 0;
		protected: 
			wxToolBar * CreateToolBar();
			void EnableTool(bool enable);
			DECLARE_CLASS(PanelTool)
	};
	
	class PanelMain: public wxPanel { 
		public: PanelMain(wxWindow *parent); 
	};

	class PanelInet: public wxPanel { 
		public: PanelInet(wxWindow *parent); 
	};

	class PanelType: public PanelTool { 
		public: 
			PanelType(wxWindow *parent); 
			virtual ~PanelType();
			virtual void Save(wxSQLite3Database &database);
		private:
			LoadThread m_thread;
			wxArrayString m_del_type;
			FbTreeViewCtrl m_treeview;
		private:
			void OnModel( FbModelEvent& event );
			void OnAppend( wxCommandEvent& event );
			void OnModify( wxCommandEvent& event );
			void OnDelete( wxCommandEvent& event );
			void OnActivated( wxTreeEvent & event );
			DECLARE_EVENT_TABLE()
	};

	class PanelRefs: public PanelTool { 
		public: 
			PanelRefs(wxWindow *parent); 
			virtual void Save(wxSQLite3Database &database);
		private:
			void OnAppend( wxCommandEvent& event );
			void OnModify( wxCommandEvent& event );
			void OnDelete( wxCommandEvent& event );
			void OnActivated( wxTreeEvent & event );
			DECLARE_EVENT_TABLE()
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
	void Assign(bool write);
private:
	FbCommonDatabase m_database;
	 wxNotebook m_notebook;
private:
	void OnSelectFolderClick( wxCommandEvent& event );
	void OnModel( FbModelEvent& event );
	DECLARE_EVENT_TABLE()
};

#endif //__FBCONFIGDLG_H__
