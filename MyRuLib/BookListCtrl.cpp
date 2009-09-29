#include <wx/imaglist.h>
#include "BookListCtrl.h"
#include "FbConst.h"
#include "FbManager.h"
#include "XpmBitmaps.h"

BEGIN_EVENT_TABLE(BookListCtrl, FbTreeListCtrl)
    EVT_UPDATE_UI(ID_EMPTY_BOOKS, BookListCtrl::OnEmptyBooks)
    EVT_UPDATE_UI(ID_APPEND_BOOK, BookListCtrl::OnAppendBook)
END_EVENT_TABLE()

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
        wxTreeItemId item = AppendItem(root, data->title, 0, -1, data);
        wxString full_name = result.GetString(wxT("full_name"));
        SetItemText (item, 1, full_name);
        SetItemText (item, 3, data->file_name);
        SetItemText (item, 4, wxString::Format(wxT("%d "), data->file_size/1024));
        do {
            result.NextRow();
            if ( data->GetId() != result.GetInt(wxT("id")) ) break;
            full_name = full_name + wxT(", ") + result.GetString(wxT("full_name"));
            SetItemText (item, 1, full_name);
        } while (!result.Eof());
    }
}

void BookListCtrl::OnEmptyBooks(wxUpdateUIEvent& event)
{
	DeleteRoot();
    wxTreeItemId root = AddRoot(wxEmptyString);
    ScrollTo(root);
}

void BookListCtrl::OnAppendBook(wxUpdateUIEvent& event)
{
	wxTreeItemId root = GetRootItem();
	wxTreeItemId item = AppendItem(root, event.GetString(), 0, -1);
}


