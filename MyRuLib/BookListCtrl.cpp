#include "BookListCtrl.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSize)
END_EVENT_TABLE()

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
