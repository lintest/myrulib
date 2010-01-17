#include <wx/imaglist.h>
#include "FbBookList.h"
#include "FbConst.h"
#include "FbBookData.h"
#include "FbLogoBitmap.h"

BEGIN_EVENT_TABLE(FbCheckList, FbTreeListCtrl)
	EVT_TREE_KEY_DOWN(wxID_ANY, FbCheckList::OnKeyDown)
	EVT_TREE_STATE_IMAGE_CLICK(wxID_ANY, FbCheckList::OnImageClick)
	EVT_TREE_ITEM_COLLAPSING(wxID_ANY, FbCheckList::OnCollapsing)
END_EVENT_TABLE()

FbCheckList::FbCheckList(wxWindow *parent, wxWindowID id, long style)
	:FbTreeListCtrl(parent, id, style)
{
	wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	AssignImageList (images);
};

void FbCheckList::SelectAll(int iImageIndex)
{
	wxTreeItemId root = GetRootItem();
	if (root.IsOk()) SelectChild(root, iImageIndex);
}

void FbCheckList::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		int image = GetItemImage(item);
		image = ( image == 1 ? 0 : 1);
		if (GetItemBold(item)) {
			SetItemImage(item, image);
			wxTreeItemIdValue cookie;
			item = GetFirstChild(item, cookie);
			while (item.IsOk()) {
				wxTreeItemIdValue cookie;
				wxTreeItemId subitem = GetFirstChild(item, cookie);
				while (subitem.IsOk()) {
					SetItemImage(subitem, image);
					subitem = GetNextSibling(subitem);
				}
				SetItemImage(item, image);
				item = GetNextSibling (item);
			}
		} else {
			SetItemImage(item, image);
			wxTreeItemId parent = GetItemParent(item);
			wxTreeItemIdValue cookie;
			item = GetFirstChild(parent, cookie);
			while (item.IsOk()) {
				if (image != GetItemImage(item)) {
					image = 2;
					break;
				}
				item = GetNextSibling (item);
			}
			SetItemImage(parent, image);
		}
	}
	event.Veto();
}

void FbCheckList::OnKeyDown(wxTreeEvent & event)
{
	if (event.GetKeyCode() == 0x20) {
		wxArrayTreeItemIds selections;
		size_t count = GetSelections(selections);
		int image = 0;
		for (size_t i=0; i<count; ++i) {
			wxTreeItemId selected = selections[i];
			if (i==0)
				image = (GetItemImage(selected) + 1) % 2;
			SetItemImage(selected, image);
		}
		event.Veto();
	} else event.Skip();
}

void FbCheckList::OnCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

void FbCheckList::SelectChild(const wxTreeItemId &parent, int iImageIndex)
{
	SetItemImage(parent, iImageIndex);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		SelectChild(child, iImageIndex);
		child = GetNextChild(parent, cookie);
	}
}

FbBookList::FbBookList(wxWindow *parent, wxWindowID id, long style)
	:FbCheckList(parent, id, style)
{
}

BookListUpdater::BookListUpdater(wxTreeListCtrl * list) :m_list(list)
{
	m_list->Freeze();
	m_list->DeleteRoot();
}

BookListUpdater::~BookListUpdater()
{
	m_list->ExpandAll( m_list->GetRootItem() );
	m_list->Thaw();
	m_list->Update();
}

size_t FbBookList::ScanChecked(const wxTreeItemId &parent, wxString &selections)
{
	size_t result = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (GetItemImage(child) == 1) {
			FbBookData * data = (FbBookData*) GetItemData(child);
			if (data && data->GetId()) {
				if ( !selections.IsEmpty() ) selections += wxT(",");
				selections += wxString::Format(wxT("%d"), data->GetId());
				result++;
			}
		}
		result += ScanChecked(child, selections);
		child = GetNextChild(parent, cookie);
	}
	return result;
}

size_t FbBookList::ScanSelected(const wxTreeItemId &parent, wxString &selections)
{
	size_t result = 0;
	wxArrayTreeItemIds itemArray;
	size_t count = FbTreeListCtrl::GetSelections(itemArray);
	for (size_t i=0; i<count; ++i) {
		FbBookData * data = (FbBookData*) GetItemData(itemArray[i]);
		if (data && data->GetId()) {
			if ( !selections.IsEmpty() ) selections += wxT(",");
			selections += wxString::Format(wxT("%d"), data->GetId());
			result++;
		}
	}
	return result;
}

void FbBookList::ScanChecked(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		if (GetItemImage(child) == 1) {
			FbBookData * data = (FbBookData*) GetItemData(child);
			if (data && data->GetId()) items.Add(data->GetId());
		}
		ScanChecked(child, items);
		child = GetNextChild(parent, cookie);
	}
}

void FbBookList::ScanSelected(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxArrayTreeItemIds itemArray;
	size_t count = FbTreeListCtrl::GetSelections(itemArray);
	for (size_t i=0; i<count; ++i) {
		FbBookData * data = (FbBookData*) GetItemData(itemArray[i]);
		if (data && data->GetId()) items.Add(data->GetId());
	}
}

wxString FbBookList::GetSelected()
{
	wxString selections;
	GetSelected(selections);
	return selections;
}

size_t FbBookList::GetSelected(wxString &selections)
{
	selections.Empty();
	wxTreeItemId root = GetRootItem();
	size_t count = ScanChecked(GetRootItem(), selections);
	if (selections.IsEmpty()) count = ScanSelected(root, selections);
	return count;
}

size_t FbBookList::GetSelected(wxArrayInt &items)
{
	items.Empty();
	wxTreeItemId root = GetRootItem();
	ScanChecked(root, items);
	if (items.IsEmpty()) ScanSelected(root, items);
	return items.Count();
}

bool FbBookList::DeleteItems(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		FbBookData * data = (FbBookData*) GetItemData(child);
		if (data && data->GetId()) {
			if (items.Index(data->GetId()) != wxNOT_FOUND) {
				Delete(child);
				return true;
			}
		} else if (!HasChildren(child)) {
			Delete(child);
			return true;
		}
		if (DeleteItems(child, items)) return true;
		child = GetNextChild(parent, cookie);
	}
	return false;
}

void FbBookList::DeleteItems(wxArrayInt &items)
{
	while (DeleteItems(GetRootItem(), items)) {};
}

size_t FbBookList::GetCount(const wxTreeItemId &parent, wxArrayInt &items)
{
	size_t count = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		FbBookData * data = (FbBookData*) GetItemData(child);
		if (data && data->GetId() && items.Index(data->GetId()) == wxNOT_FOUND) {
			items.Add(data->GetId());
			count++;
		}
		count += GetCount(child, items);
		child = GetNextChild(parent, cookie);
	}
	return count;
}

size_t FbBookList::GetCount()
{
	wxArrayInt items;
	return GetCount(GetRootItem(), items);
}

