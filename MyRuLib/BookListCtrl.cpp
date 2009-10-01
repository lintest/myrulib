#include <wx/imaglist.h>
#include "BookListCtrl.h"
#include "FbConst.h"
#include "FbBookData.h"
#include "XpmBitmaps.h"

BookListCtrl::BookListCtrl(wxWindow *parent, wxWindowID id, long style)
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

void BookListCtrl::SelectChild(const wxTreeItemId &parent, int iImageIndex)
{
    SetItemImage(parent, iImageIndex);
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while (child.IsOk()) {
        SelectChild(child, iImageIndex);
        child = GetNextChild(parent, cookie);
    }
}

void BookListCtrl::SelectAll(int iImageIndex)
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

void BookListCtrl::FillBooks(wxSQLite3ResultSet & result, const wxString &caption)
{
	BookListUpdater updater(this);

    wxTreeItemId root = AddRoot(caption);
    ScrollTo(root);

    while (!result.Eof()) {
        BookTreeItemData * data = new BookTreeItemData(result);
        wxString full_name = result.GetString(wxT("full_name"));
        do {
            result.NextRow();
            if ( data->GetId() != result.GetInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
        } while (!result.Eof());
        wxTreeItemId item = AppendItem(root, data->title, 0, -1, data);
        SetItemText (item, 1, full_name);
        SetItemText (item, 2, data->file_name);
        SetItemText (item, 3, wxString::Format(wxT("%d "), data->file_size/1024));
    }
}

void BookListCtrl::ScanChecked(const wxTreeItemId &root, wxString &selections)
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

void BookListCtrl::ScanSelected(const wxTreeItemId &root, wxString &selections)
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

wxString BookListCtrl::GetSelected()
{
    wxString selections;
    wxTreeItemId root = GetRootItem();
    ScanChecked(root, selections);
    if (selections.IsEmpty()) ScanSelected(root, selections);
    return selections;
}

