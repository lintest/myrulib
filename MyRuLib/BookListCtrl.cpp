#include "BookListCtrl.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSize)
END_EVENT_TABLE()

void BookListCtrl::OnSize(wxSizeEvent& event)
{
	event.Skip();

	int sum = 0;
	for (size_t i = 0; i<colSizes.Count() && i<GetColumnCount(); i++) {
        sum += colSizes[i];
	}

	if (!sum) return;

	int w = GetClientSize().x - 20;
	for (size_t i = 0; i<colSizes.Count() && i<GetColumnCount(); i++) {
        SetColumnWidth(i, w * colSizes[i] / sum );
	}
}
