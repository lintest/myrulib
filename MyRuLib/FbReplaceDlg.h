#ifndef __FBREPLACEDLG_H__
#define __FBREPLACEDLG_H__

#include "FbWindow.h"
#include "FbTreeListCtrl.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbMasterList.h"

class FbReplaceDlg : public FbDialog
{
	public:
		FbReplaceDlg(const wxString& title, int id);
		static int Execute(int author, wxString& newname);
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
		FbMasterList* m_FindList;
	private:
		void Init();
		int GetSelected();
		bool Load();
		int DoUpdate();
		wxString GetFullName();
		FbCommonDatabase m_database;
		int m_id;
	private:
		void OnFindEnter( wxCommandEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif //__FBREPLACEDLG_H__
