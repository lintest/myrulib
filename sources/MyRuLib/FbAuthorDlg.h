#ifndef __FbAuthorModifyDlg_H__
#define __FbAuthorModifyDlg_H__

#include "FbWindow.h"
#include "controls/FbTreeView.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbThread.h"
#include "FbImportCtx.h"

class FbAuthorReplaceDlg : public FbDialog
{
	public:
		FbAuthorReplaceDlg(const wxString& title, int id);
		virtual ~FbAuthorReplaceDlg();
		static int Execute(int author, wxString& newname);
		static bool Delete(FbModel &model);
	protected:
		virtual void EndModal(int retCode);
	protected:
		enum
		{
			ID_TEXT = 1000,
			ID_FIND_TXT,
			ID_FIND_BTN,
			ID_FIND_LIST,
		};
		wxTextCtrl* m_Text;
		wxTextCtrl* m_FindText;
		wxBitmapButton* m_FindBtn;
		FbTreeViewCtrl * m_FindList;
	private:
		void Init();
		int GetSelected();
		bool Load();
		int DoUpdate();
		wxString GetFullName();
		FbCommonDatabase m_database;
		int m_id;
		FbThread * m_thread;
	private:
		void OnFindEnter( wxCommandEvent& event );
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
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

#endif //__FbAuthorModifyDlg_H__
