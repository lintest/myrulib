#include "BookListCtrl.h"
#include <wx/imaglist.h>
#include "XpmBitmaps.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSize)
END_EVENT_TABLE()

BookListCtrl::BookListCtrl(wxWindow *parent, wxWindowID id, long style)
    :wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
    AddColumn (_T("Заголовок"), 300, wxALIGN_LEFT);
    AddColumn (_T("№"), 30, wxALIGN_LEFT);
    AddColumn (_T("Имя файла"), 100, wxALIGN_LEFT);
    AddColumn (_T("Размер, Кб"), 100, wxALIGN_RIGHT);
    SetColumnEditable (0, false);
    SetColumnEditable (1, false);
    SetColumnEditable (2, false);
    SetColumnEditable (3, false);
    colSizes.Add(9);
    colSizes.Add(1);
    colSizes.Add(4);
    colSizes.Add(2);

    wxBitmap size = wxBitmap(checked_xpm);
	wxImageList *images;
	images = new wxImageList (size.GetWidth(), size.GetHeight(), true);
	images->Add (wxBitmap(nocheck_xpm));
	images->Add (wxBitmap(checked_xpm));
	images->Add (wxBitmap(checkout_xpm));
	AssignImageList (images);
};

void BookListCtrl::OnSize(wxSizeEvent& event)
{
	event.Skip();

	int sum = 0;
	for (size_t i = 0; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        sum += colSizes[i];
	}

	if (!sum) return;

	int w = GetClientSize().x - 20;
	for (size_t i = 0; i<(size_t)colSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        SetColumnWidth(i, w * colSizes[i] / sum );
	}
}

void BookListCtrl::SelectChild(const wxTreeItemId &parent)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    while (child.IsOk()) {
        SetItemImage(child, 1);
        SelectChild(child);
        child = GetNextChild(parent, cookie);
    }
}

void BookListCtrl::SelectAll()
{
    wxTreeItemId root = GetRootItem();
    if (root.IsOk()) SelectChild(root);
}
