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

void FbCheckList::SelectAll(int image)
{
	wxTreeItemId root = GetRootItem();
	if (root.IsOk()) SelectChild(root, image);
}

void FbCheckList::SelectChild(const wxTreeItemId &parent, int image)
{
	SetItemImage(parent, image);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		SelectChild(child, image);
		child = GetNextChild(parent, cookie);
	}
}

void FbCheckList::SelectParent(const wxTreeItemId &item)
{
	wxTreeItemId parent = GetItemParent(item);
	if (!parent.IsOk()) return;

	int all = -1;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int image = GetItemImage(child);
		if (all < 0) all = image; else if (all != image) all = 2;
		child = GetNextChild(parent, cookie);
	}
	SetItemImage(parent, all);
	SelectParent(parent);
}

void FbCheckList::OnImageClick(wxTreeEvent &event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		int image = GetItemImage(selected);
		image = ( image == 1 ? 0 : 1);
		SelectChild(selected, image);
		SelectParent(selected);
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
			if (i==0) image = (GetItemImage(selected) + 1) % 2;
			SelectChild(selected, image);
			SelectParent(selected);
		}
		event.Veto();
	} else event.Skip();
}

void FbCheckList::OnCollapsing(wxTreeEvent & event)
{
	event.Veto();
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

int FbBookList::GetItemBook(const wxTreeItemId &item) const
{
	FbItemData * data = (FbItemData*) GetItemData(item);
	return data ? data->GetId() : 0;
}

size_t FbBookList::ScanChecked(const wxTreeItemId &parent, wxString &selections)
{
	size_t result = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		int id;
		if ( GetItemImage(child) == 1 && (id = GetItemBook(child)) ) {
			if ( !selections.IsEmpty() ) selections += wxT(",");
			selections += wxString::Format(wxT("%d"), id);
			result++;
		}
		result += ScanChecked(child, selections);
		child = GetNextChild(parent, cookie);
	}
	return result;
}

size_t FbBookList::ScanSelected(const wxTreeItemId &parent, wxString &selections)
{
	size_t result = 0;
	wxArrayTreeItemIds array;
	size_t count = FbTreeListCtrl::GetSelections(array);
	for (size_t i=0; i<count; ++i) {
		int id;
		if (id = GetItemBook(array[i])) {
			if ( !selections.IsEmpty() ) selections += wxT(",");
			selections += wxString::Format(wxT("%d"), id);
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
		int id;
		if ( GetItemImage(child) == 1 && (id = GetItemBook(child)) ) items.Add(id);
		ScanChecked(child, items);
		child = GetNextChild(parent, cookie);
	}
}

void FbBookList::ScanSelected(const wxTreeItemId &parent, wxArrayInt &items)
{
	wxArrayTreeItemIds array;
	size_t count = FbTreeListCtrl::GetSelections(array);
	for (size_t i=0; i<count; ++i) {
		int id;
		if (id = GetItemBook(array[i])) items.Add(id);
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
		FbItemData * data = (FbItemData*) GetItemData(child);
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
		FbItemData * data = (FbItemData*) GetItemData(child);
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

