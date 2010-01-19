#ifndef __FBCHECKLIST_H__
#define __FBCHECKLIST_H__

#include "FbTreeListCtrl.h"

class FbCheckList: public FbTreeListCtrl
{
	public:
		FbCheckList(wxWindow *parent, wxWindowID id, long style);
		void SelectAll(int image = 1);
	private:
		void SelectChild(const wxTreeItemId &parent, int image);
		void SelectParent(const wxTreeItemId &item);
	private:
		void OnKeyDown(wxTreeEvent & event);
		void OnCollapsing(wxTreeEvent & event);
		void OnImageClick(wxTreeEvent &event);
		DECLARE_EVENT_TABLE();
};

#endif // __FBCHECKLIST_H__
