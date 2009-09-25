#include <wx/imaglist.h>
#include "BookListCtrl.h"
#include "FbConst.h"
#include "FbManager.h"
#include "XpmBitmaps.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSizing)
END_EVENT_TABLE()

BookListCtrl::BookListCtrl(wxWindow *parent, wxWindowID id, long style)
    :wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
    wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	AssignImageList (images);
};

void BookListCtrl::AddColumn (const wxString& text, int width, int flag)
{
    wxTreeListCtrl::AddColumn(text, width, flag, -1, true, false);
    colSizes.Add(width);
}

void BookListCtrl::OnSizing(wxSizeEvent& event)
{
	int sum = 0;
	for (size_t i = 0; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        sum += colSizes[i];
	}

	if (!sum) return;

	int w = event.GetSize().GetWidth() - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) - 6;
    int xx = w;
	for (size_t i = 1; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
	    int x = w * colSizes[i] / sum;
        SetColumnWidth(i, x);
        xx -= x;
	}
    SetColumnWidth(0, xx);
	event.Skip();
}

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

