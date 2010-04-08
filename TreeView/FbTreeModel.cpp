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

const wxBitmap & FbTreeModel::GetBitmap(const FbTreeItemId &id) 
{
	int state = GetState(id);
	if (state = wxNOT_FOUND) return wxNullBitmap;

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
	if (count) m_current = FbTreeItemKeyList(1);
}

void FbTreeModelList::SetRowCount(size_t count) 
{ 
	m_count = count; 
	if (count && !m_current.IsOk()) m_current = FbTreeItemKeyList(1);
}


void FbTreeModelList::DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	for ( ; pos < count && y < yy; pos++, y+=h )
	{
        FbTreeItemId id = FbTreeItemKeyList(pos + 1);

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

        rect.Deflate(3, 1);
		wxString text = GetValue(id, 0);
		dc.DrawLabel(text, GetBitmap(id), rect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	}
}

int FbTreeModelList::GoFirstRow()
{
	size_t count = GetRowCount();

	if (count) {
	    m_current = FbTreeItemId(FbTreeItemKeyList(1));
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
	    m_current = FbTreeItemId(FbTreeItemKeyList(count));
	    return count - 1;
    } else {
        m_current = FbTreeItemId();
        return wxNOT_FOUND;
    }
}

int FbTreeModelList::GoNextRow(size_t delta)
{
	size_t count = GetRowCount();
	FbTreeItemKeyList * key = FbTreeItemKeyList::Key(m_current);
	if (count && key) {
		size_t rowid = key->GetId();
		rowid  = rowid + delta <= count ? rowid + delta : count;
		m_current = FbTreeItemKeyList(rowid);
		return rowid - 1;
	}
	return GoFirstRow();
}

int FbTreeModelList::GoPriorRow(size_t delta)
{
	size_t count = GetRowCount();
	FbTreeItemKeyList * key = FbTreeItemKeyList::Key(m_current);
	if (count && key) {
		size_t rowid = key->GetId();
		rowid  = rowid <= delta ? 1 : rowid - delta;
		m_current = FbTreeItemKeyList(rowid);
		return rowid - 1;
	}
	return GoFirstRow();
}

FbTreeItemId FbTreeModelList::FindItem(size_t row, bool select)
{
	if (row<GetRowCount()) {
		FbTreeItemId res = FbTreeItemKeyList(row + 1);
		if (select) m_current = res;
		return res;
	}
	return FbTreeItemId();
}

wxString FbTreeModelList::GetValue(const FbTreeItemId &id, size_t col)
{
	FbTreeItemKeyList * key = FbTreeItemKeyList::Key(id);
	if (key) {
		size_t rowid = key->GetId();
        wxString text = wxString::Format(_("Paint %d"), rowid);
        for (size_t j=0; j < rowid % 30; j++) text += _(" 0");
		return text;
	}
	return wxEmptyString;
}

