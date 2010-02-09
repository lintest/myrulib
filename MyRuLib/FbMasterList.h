#ifndef __FBMASTERLIST_H__
#define __FBMASTERLIST_H__

#include "FbTreeListCtrl.h"
#include "FbMasterData.h"

class FbMasterEvent;

class FbMasterList: public FbTreeListCtrl
{
	public:
		FbMasterList(wxWindow *parent, wxWindowID id, long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER);
		FbMasterData * GetSelectedData() const;
		FbMasterData * GetItemData (const wxTreeItemId& item) const;
		void DeleteItem(FbMasterData &data, wxTreeItemId parent = 0L);
	private:
		void OnEmptyMasters(FbMasterEvent& event);
		void OnAppendMaster(FbMasterEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBMASTERLIST_H__
