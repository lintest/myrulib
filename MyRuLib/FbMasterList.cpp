#include "FbMasterList.h"
#include "FbConst.h"
#include "FbBookEvent.h"

BEGIN_EVENT_TABLE(FbMasterList, FbTreeListCtrl)
	EVT_FB_MASTER(ID_EMPTY_MASTERS, FbMasterList::OnEmptyMasters)
	EVT_FB_MASTER(ID_APPEND_MASTER, FbMasterList::OnAppendMaster)
END_EVENT_TABLE()

FbMasterList::FbMasterList(wxWindow *parent, wxWindowID id, long style)
	:FbTreeListCtrl(parent, id, style)
{
}

void FbMasterList::OnAppendMaster(FbMasterEvent& event)
{
}

void FbMasterList::OnEmptyMasters(FbMasterEvent& event)
{
	FbTreeListUpdater updater(this);
	DeleteRoot();
	AddRoot(wxEmptyString);
}

FbMasterData * FbMasterList::GetSelectedData() const
{
    wxTreeItemId selected = GetSelection();
    return selected.IsOk() ? GetItemData(selected) : NULL;
}

FbMasterData * FbMasterList::GetItemData(const wxTreeItemId& item) const
{
    return (FbMasterData*) FbTreeListCtrl::GetItemData(item);
}

void FbMasterList::DeleteItem(FbMasterData &data, wxTreeItemId parent)
{
/*
	if (!parent.IsOk()) parent = GetRootItem();
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		FbMasterData * value = (FbMasterData*) GetItemData(child);
		if (value && *value == data) {
			Delete(child);
			return;
		}
		child = GetNextChild(parent, cookie);
	}
*/
}

