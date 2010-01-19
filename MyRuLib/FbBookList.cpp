#include "FbBookList.h"
#include "FbBookData.h"

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
		int id = GetItemBook(child);
		if ( id ) {
			if (items.Index(id) != wxNOT_FOUND) {
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
		int id = GetItemBook(child);
		if (id && items.Index(id) == wxNOT_FOUND) {
			items.Add(id);
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

