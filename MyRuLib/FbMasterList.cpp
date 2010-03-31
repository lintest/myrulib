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
	FbTreeListUpdater updater(this);
	wxTreeItemId root = GetRootItem();

	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(root, cookie);

	wxTreeItemId item = AppendItem(root, event.GetString(), -1, -1, event.m_data);
	if (event.m_number) {
		wxString number = wxString::Format(wxT("%d "), event.m_number);
		SetItemText(item, 1, number);
	}

	if (!child.IsOk()) SelectItem(item);
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
}

