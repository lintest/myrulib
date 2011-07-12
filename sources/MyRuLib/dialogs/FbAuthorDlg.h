#ifndef __FBAUTHORDLG_H__
#define __FBAUTHORDLG_H__

#include "FbWindow.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbThread.h"
#include "FbImportCtx.h"
#include "FbAlphabet.h"
#include "controls/FbTreeView.h"
#include "controls/FbSearchCombo.h"

class FbAuthorSelectDlg : public wxDialog 
{
	private:
		enum ID {
			ID_ALPHABET = 1000,
			ID_FIND_TEXT,
			ID_TREE_VIEW,
			ID_APPEND,
			ID_MODIFY,
			ID_DELETE,
		};
	public:
		static int Execute( wxWindow* parent, int id );
		FbAuthorSelectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 329,321 ), long style = wxDEFAULT_DIALOG_STYLE );
		~FbAuthorSelectDlg();
	private:
		wxTextCtrl m_text;
		FbAlphabetCombo m_alphabet;
		wxToolBar m_toolbar;
		FbTreeViewCtrl m_treeview;
};

class FbAuthorReplaceDlg : public FbDialog
{
	public:
		FbAuthorReplaceDlg(const wxString& title, int id);
		virtual ~FbAuthorReplaceDlg();
		static int Execute(int author, wxString& newname, const wxString& counter);
		static bool Delete(FbModel &model);
	protected:
		virtual void EndModal(int retCode);
	protected:
		enum
		{
			ID_TEXT = 1000,
			ID_FIND_TXT,
			ID_FIND_LIST,
		};
		wxTextCtrl * m_Text;
		FbSearchCombo * m_FindText;
		FbTreeViewCtrl * m_MasterList;
	private:
		void Init();
		int GetSelected();
		bool Load();
		int DoUpdate();
		wxString GetFullName();
		FbCommonDatabase m_database;
		int m_id;
		FbThread * m_thread;
		wxString m_MasterFile;
		wxString m_MasterTemp;
	private:
		void OnFindEnter( wxCommandEvent& event );
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
		void OnNumber(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

class FbAuthorModifyDlg : public FbDialog
{
	public:
		FbAuthorModifyDlg( const wxString& title = wxEmptyString, int id = 0 );
		static int Append(wxString &newname);
		static int Modify(int id, wxString &newname);
		static void ReplaceAuthor(int old_id, int new_id);
	protected:
		virtual void EndModal(int retCode);
	private:
		enum
		{
			ID_LAST_NAME = 1000,
			ID_FIRST_NAME,
			ID_MIDDLE_NAME,
		};
		bool Load(int id);
		wxTextCtrl * AppenName(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption);
		void SetValue(wxWindowID id, const wxString &text);
		wxString GetValue(wxWindowID id);
		int FindAuthor();
		int DoUpdate();
		int DoAppend();
		void DoModify();
		void GetValues(AuthorItem &author);
		wxString GetFullName();
	private:
		FbCommonDatabase m_database;
		int m_id;
		int m_exists;
};

#endif // __FBAUTHORDLG_H__
