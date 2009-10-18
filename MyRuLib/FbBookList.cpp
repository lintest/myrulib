#include <wx/imaglist.h>
#include "FbBookList.h"
#include "FbConst.h"
#include "FbBookData.h"
#include "XpmBitmaps.h"

FbBookList::FbBookList(wxWindow *parent, wxWindowID id, long style)
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

void FbBookList::SelectChild(const wxTreeItemId &parent, int iImageIndex)
{
    SetItemImage(parent, iImageIndex);
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while (child.IsOk()) {
        SelectChild(child, iImageIndex);
        child = GetNextChild(parent, cookie);
    }
}

void FbBookList::SelectAll(int iImageIndex)
{
    wxTreeItemId root = GetRootItem();
    if (root.IsOk()) SelectChild(root, iImageIndex);
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

void FbBookList::ScanChecked(const wxTreeItemId &root, wxString &selections)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(root, cookie);
    while (child.IsOk()) {
        if (GetItemImage(child) == 1) {
            BookTreeItemData * data = (BookTreeItemData*) GetItemData(child);
            if (data && data->GetId()) {
                if ( !selections.IsEmpty() ) selections += wxT(",");
                selections += wxString::Format(wxT("%d"), data->GetId());
            }
        }
        ScanChecked(child, selections);
        child = GetNextChild(root, cookie);
    }
}

void FbBookList::ScanSelected(const wxTreeItemId &root, wxString &selections)
{
    wxArrayTreeItemIds itemArray;
    size_t count = FbTreeListCtrl::GetSelections(itemArray);
    for (size_t i=0; i<count; ++i) {
        BookTreeItemData * data = (BookTreeItemData*) GetItemData(itemArray[i]);
        if (data && data->GetId()) {
            if ( !selections.IsEmpty() ) selections += wxT(",");
            selections += wxString::Format(wxT("%d"), data->GetId());
        }
    }
}

void FbBookList::ScanChecked(const wxTreeItemId &root, wxArrayInt &items)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(root, cookie);
    while (child.IsOk()) {
        if (GetItemImage(child) == 1) {
            BookTreeItemData * data = (BookTreeItemData*) GetItemData(child);
            if (data && data->GetId()) items.Add(data->GetId());
        }
        ScanChecked(child, items);
        child = GetNextChild(root, cookie);
    }
}

void FbBookList::ScanSelected(const wxTreeItemId &root, wxArrayInt &items)
{
    wxArrayTreeItemIds itemArray;
    size_t count = FbTreeListCtrl::GetSelections(itemArray);
    for (size_t i=0; i<count; ++i) {
        BookTreeItemData * data = (BookTreeItemData*) GetItemData(itemArray[i]);
        if (data && data->GetId()) items.Add(data->GetId());
    }
}

wxString FbBookList::GetSelected()
{
    wxString selections;
    wxTreeItemId root = GetRootItem();
    ScanChecked(GetRootItem(), selections);
    if (selections.IsEmpty()) ScanSelected(root, selections);
    return selections;
}

void FbBookList::GetSelected(wxArrayInt &items)
{
    wxString selections;
    wxTreeItemId root = GetRootItem();
    ScanChecked(root, items);
    if (selections.IsEmpty()) ScanSelected(root, items);
}

bool FbBookList::DeleteItems(const wxTreeItemId &root, wxArrayInt &items)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(root, cookie);
    while (child.IsOk()) {
		BookTreeItemData * data = (BookTreeItemData*) GetItemData(child);
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
        child = GetNextChild(root, cookie);
    }
	return false;
}

void FbBookList::DeleteItems(wxArrayInt &items)
{
	while (DeleteItems(GetRootItem(), items)) {};
}
