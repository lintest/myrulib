#ifndef __FBAUTHORDLG_H__
#define __FBAUTHORDLG_H__

#include "FbWindow.h"
#include "ImpContext.h"
#include "FbDatabase.h"

class FbAuthorDlg : public FbDialog
{
	public:
		FbAuthorDlg( const wxString& title = wxEmptyString, int id = 0 );
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

#endif //__FBAUTHORDLG_H__
