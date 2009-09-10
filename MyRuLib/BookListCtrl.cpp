#include "BookListCtrl.h"
#include "FbConst.h"
#include <wx/imaglist.h>
#include "XpmBitmaps.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
	EVT_MENU(wxID_SELECTALL, BookListCtrl::OnSelectAll)
	EVT_MENU(ID_UNSELECTALL, BookListCtrl::OnUnselectAll)
    EVT_SIZE(BookListCtrl::OnSize)
//    EVT_CONTEXT_MENU(BookListCtrl::OnContextMenu)
    EVT_TREE_ITEM_MENU(wxID_ANY, BookListCtrl::OnContextMenu)
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

void BookListCtrl::OnSize(wxSizeEvent& event)
{
	event.Skip();

	int sum = 0;
	for (size_t i = 0; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        sum += colSizes[i];
	}

	if (!sum) return;

	int w = GetClientSize().x - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	for (size_t i = 0; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        SetColumnWidth(i, w * colSizes[i] / sum );
	}
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

void BookListCtrl::OnContextMenu(wxTreeEvent& event)
{
    wxPoint point = event.GetPoint();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 3;
        point.y = size.y / 3;
    }
    ShowContextMenu(point);
}

void BookListCtrl::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;


	menu.Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu.Append(ID_UNSELECTALL, _("Отменить выделение"));
    menu.Append(wxID_ANY, _T("&Test"));
    menu.Append(wxID_ANY, _T("&About"));
    menu.AppendSeparator();
    menu.Append(wxID_ANY, _T("E&xit"));

    PopupMenu(&menu, pos.x, pos.y);
}

void BookListCtrl::OnSelectAll(wxCommandEvent& event)
{
    SelectAll();
}

void BookListCtrl::OnUnselectAll(wxCommandEvent& event)
{
    SelectAll(0);
}

