#pragma once

#include "FbWindow.h"
#include "FbDatabase.h"

class FbSequenDlg: public FbDialog
{
	public:
		FbSequenDlg( const wxString& title = wxEmptyString, int id = 0 );
		static int Append(wxString &newname);
		static int Modify(int id, wxString &newname);
	protected:
		virtual void EndModal(int retCode);
	private:
		bool Load(int id);
		int Find();
		int DoAppend();
		int DoUpdate();
		int DoModify();
		int DoReplace();
		wxString GetValue() { return m_edit.GetValue().Trim(false).Trim(true); };
	private:
		wxTextCtrl m_edit;
		wxStaticText m_text;
		FbCommonDatabase m_database;
		int m_id;
		int m_exists;
};
