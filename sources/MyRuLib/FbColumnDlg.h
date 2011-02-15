#ifndef __FBCOLUMNDLG_H__
#define __FBCOLUMNDLG_H__

#include <wx/wx.h>
#include "FbWindow.h"
#include "controls/FbTreeView.h"

class FbColumnModel: public FbListStore
{
	public:
		void MoveUp();
		void MoveDown();
};

class FbColumnDlg : public FbDialog
{
	public:
		static bool Execute(wxWindow * parent, wxArrayInt & columns);
		FbColumnDlg(wxWindow * parent, const wxArrayInt & columns);
	private:
		FbModel * CreateModel(const wxArrayInt & columns);
		void GetColumns(wxArrayInt & columns);
		void DoSelected();
	private:
		FbTreeViewCtrl m_fields;
		wxToolBar m_toolbar;
		FbColumnModel * m_model;
	private:
		void OnItemSelected(wxTreeEvent & event);
		void OnMoveUp(wxCommandEvent& event);
		void OnMoveDown(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBCOLUMNDLG_H__
