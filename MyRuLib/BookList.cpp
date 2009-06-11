#include "BookList.h"

BEGIN_EVENT_TABLE(BookListCtrl, wxTreeListCtrl)
    EVT_SIZE(BookListCtrl::OnSize)
END_EVENT_TABLE()

void BookListCtrl::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);
	w -= 20;
    SetColumnWidth(0, w * 5 / 8);
    SetColumnWidth(1, w * 2 / 8);
    SetColumnWidth(2, w / 8);
	event.Skip();
}

