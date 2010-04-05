#include "FbTreeModel.h"

#include "../MyRuLib/FbLogoBitmap.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbColumnArray);

FbTreeModel::FbTreeModel() :
    m_owner(NULL),
    // Set brush colour
    m_normalBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX), wxSOLID),
    m_hilightBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID),
    m_unfocusBrush(wxSystemSettings::GetColour (wxSYS_COLOUR_BTNSHADOW), wxSOLID),
    // Set font colour
    m_normalColour(wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT)),
	m_hilightColour(wxSystemSettings::GetColour (wxSYS_COLOUR_HIGHLIGHTTEXT))
{

}

const wxBitmap & FbTreeModel::GetBitmap(int state) const
{
	static wxBitmap bitmaps[3] = {
		wxBitmap(nocheck_xpm),
		wxBitmap(checked_xpm),
		wxBitmap(checkout_xpm),
	};

	return bitmaps[state % 3];
}


FbTreeModelList::FbTreeModelList(size_t count):
	m_count(count)
{
	FbTreeItemKeyList key = 10;
	m_current = key;
}


void FbTreeModelList::DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	for ( ; pos < count && y < yy; pos++, y+=h )
	{
        wxString text = wxString::Format(_("Paint %d"), pos);
        for (size_t j=0; j<pos%30; j++) text += _(" 0");

        FbTreeItemKeyList key = pos;
        FbTreeItemId id = key;

		if (id == m_current) {
			if (wxWindow::FindFocus() == m_owner) {
                dc.SetBrush(m_hilightBrush);
            } else {
                dc.SetBrush(m_unfocusBrush);
            }
            dc.SetTextForeground(m_hilightColour);
		} else {
			dc.SetBrush (m_normalBrush);
			dc.SetTextForeground(m_normalColour);
		}
        dc.SetPen (*wxTRANSPARENT_PEN);

		wxRect rect(0, y, ww, h);

		dc.DrawRectangle(rect);

        rect.Deflate(2, 2);
		dc.DrawLabel(text, GetBitmap(pos), rect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	}
}

int FbTreeModelList::GoFirstRow()
{
	size_t count = GetRowCount();

	if (count) {
	    m_current = FbTreeItemId(FbTreeItemKeyList(0));
	    return 0;
    } else {
        m_current = FbTreeItemId();
        return wxNOT_FOUND;
    }
}

int FbTreeModelList::GoLastRow()
{
	size_t count = GetRowCount();

	if (count) {
	    m_current = FbTreeItemId(FbTreeItemKeyList(count - 1));
	    return count - 1;
    } else {
        m_current = FbTreeItemId();
        return wxNOT_FOUND;
    }
}

int FbTreeModelList::GoNextRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count && m_current.GetKeyType() == FbTreeItemKey::KT_LIST) {
		size_t rowid = ((FbTreeItemKeyList*)m_current.GetKey())->GetId();
		rowid  = rowid + delta < count ? rowid + delta : count - 1;
		m_current = FbTreeItemKeyList(rowid);
		return rowid;
	}
	return GoFirstRow();
}

int FbTreeModelList::GoPriorRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count && m_current.GetKeyType() == FbTreeItemKey::KT_LIST) {
		size_t rowid = ((FbTreeItemKeyList*)m_current.GetKey())->GetId();
		rowid  = rowid < delta ? 0 : rowid - delta;
		m_current = FbTreeItemKeyList(rowid);
		return rowid;
	}
	return GoFirstRow();
}

