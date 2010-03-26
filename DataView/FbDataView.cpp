#include "FbDataView.h"
#include "FbDataModel.h"

BEGIN_EVENT_TABLE( FbDataViewCtrl, wxDataViewCtrl )
	EVT_SIZE(FbDataViewCtrl::OnSize)
END_EVENT_TABLE()

void FbDataViewCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
/*
    ClearColumns();

    int width = GetClientSize().GetWidth();

	int flags = wxDATAVIEW_COL_RESIZABLE;

    FbTitleRenderer *cr = new FbTitleRenderer;
    wxDataViewColumn *column = new wxDataViewColumn("title", cr, 0, width-150, wxALIGN_LEFT, flags );
    AppendColumn( column );

    AppendTextColumn(_("rowid"), 1, wxDATAVIEW_CELL_ACTIVATABLE, 50, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
    AppendTextColumn(_("book"),  2, wxDATAVIEW_CELL_ACTIVATABLE, 50, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
    AppendTextColumn(_("size"),  3, wxDATAVIEW_CELL_ACTIVATABLE, 50, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
*/
    Refresh();
}
