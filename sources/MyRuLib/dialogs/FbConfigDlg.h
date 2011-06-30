#ifndef __FBCONFIGDLG_H__
#define __FBCONFIGDLG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/arrimpl.cpp>
#include <wx/gbsizer.h>
#include "FbBookEvent.h"
#include "controls/FbTreeModel.h"
#include "controls/FbTreeView.h"
#include "controls/FbComboBox.h"
#include "controls/FbChoiceCtrl.h"
#include "FbWindow.h"
#include "FbDatabase.h"

class FbDirectoryDlg: public FbDialog  
{
	public:
		FbDirectoryDlg( wxWindow * parent, const wxString& title ); 
		void Set(const FbModelItem & item);
		void Get(FbModelItem & item);

		class CreateDlg: public FbDialog {
			public:
				static bool Execute(wxWindow * parent, wxString & name, wxString & type);
				CreateDlg( wxWindow * parent );
			private:
				wxTextCtrl * m_name;
				FbChoiceStr * m_type;
		};

	private:
		enum ID {
			ID_TITLE = 1000,
			ID_DIR_FILE,
			ID_DIR_DATA,
			ID_DIR_TYPE,
			ID_DIR_CODE,
			ID_DIR_NAME,
			ID_DIR_INFO,
			ID_DIR_PRNT,
			ID_REF_FILE,
			ID_REF_DATA,
			ID_REF_TYPE,
			ID_REF_CODE,
			ID_REF_BOOK,
			ID_FB2_CODE,
			ID_LAST,
		};
		void Append( wxFlexGridSizer * sizer, wxControl * control, const wxString & title );
	private:
		static FbChoiceStr * CreateDirType(wxWindow *parent, wxWindowID winid);
		static FbChoiceStr * CreateRefType(wxWindow *parent, wxWindowID winid);
		wxGridBagSizer * m_sizer;
};

class FbConfigDlg : private FbDialog
{
	public:
		static bool Execute(wxWindow * parent);
		static wxString GetFields();
		FbConfigDlg(wxWindow * parent);

	private:
		enum ID {
			ID_LIBRARY_TITLE = 1000,
			ID_LIBRARY_DIR,
			ID_LIBRARY_DESCR,
			ID_DOWNLOAD_HOST,
			ID_DOWNLOAD_USER,
			ID_DOWNLOAD_PASS,
			ID_TYPE_LIST,
			ID_REFS_LIST,
			ID_CREATE,
			ID_APPEND,
			ID_MODIFY,
			ID_DELETE,
		};

		class TypeThread: public FbThread {
			public:
				TypeThread(wxEvtHandler * frame)
					: FbThread(wxTHREAD_JOINABLE), m_frame(frame) {}
			protected:
				virtual void * Entry();
			private:
				void LoadTypes(wxSQLite3Database &database);
				wxEvtHandler * m_frame;
		};
		
		class RefsThread: public FbThread {
			public:
				RefsThread(wxEvtHandler * frame)
					: FbThread(wxTHREAD_JOINABLE), m_frame(frame) {}
			protected:
				virtual void * Entry();
			private:
				void LoadTables(wxSQLite3Database &database);
				wxEvtHandler * m_frame;
		};
		
		class PanelTool: public wxPanel { 
			public: 
				PanelTool(wxWindow * parent); 
				virtual void Save(wxSQLite3Database &database) = 0;
			protected: 
				wxToolBar m_toolbar;
				FbTreeViewCtrl m_treeview;
				void EnableTool(bool enable);
			private:
				void OnModel( FbModelEvent& event );
				DECLARE_CLASS(PanelTool)
				DECLARE_EVENT_TABLE()
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
				TypeThread m_thread;
				wxArrayString m_deleted;
			private:
				void OnAppend( wxCommandEvent& event );
				void OnModify( wxCommandEvent& event );
				void OnDelete( wxCommandEvent& event );
				void OnActivated( wxTreeEvent & event );
				DECLARE_EVENT_TABLE()
		};

		class PanelRefs: public PanelTool { 
			public: 
				PanelRefs(wxWindow *parent, FbDatabase & database); 
				virtual ~PanelRefs();
				virtual void Save(wxSQLite3Database &database);
			private:
				RefsThread m_thread;
				FbDatabase & m_database;
				wxArrayInt m_deleted;
			private:
				void OnCreate( wxCommandEvent& event );
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
				DECLARE_CLASS(TypeData);
		};

		class RefsData: public FbModelData
		{
			public:
				RefsData(wxSQLite3ResultSet &result);
				RefsData(const wxArrayString & values);
				int GetCode() const { return m_code; }
				int Assign(wxSQLite3Statement & stmt);
				bool IsModified() { return m_modified; }
			public:
				virtual void SetValue(FbModel & model, size_t col, const wxString &value);
				virtual wxString GetValue(FbModel & model, size_t col = 0) const;
			protected:
				int m_code;
				wxArrayString m_values;
				bool m_modified;
				DECLARE_CLASS(RefsData);
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
