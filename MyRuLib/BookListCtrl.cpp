#include "BookListCtrl.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSize)
END_EVENT_TABLE()

void BookListCtrl::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);
	w -= 20;
    SetColumnWidth(0, w * 9 / 16);
    SetColumnWidth(1, w * 1 / 16);
    SetColumnWidth(2, w * 4 / 16);
    SetColumnWidth(3, w * 2 / 16);
	event.Skip();
}

