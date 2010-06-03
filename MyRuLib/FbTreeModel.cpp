#include "FbTreeModel.h"
#include <wx/wxsqlite3.h>
#include <wx/listbase.h>

#include "../MyRuLib/FbLogoBitmap.h"

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbColumnInfo, wxObject)

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbModelData, wxObject)

static void Assign(wxSQLite3ResultSet &res, const wxString& column, int &value)
{
    for (int i=0; i<res.GetColumnCount(); i++) {
        if (res.GetColumnName(i).CmpNoCase(column)==0) {
            value = res.GetInt(i);
            return;
        }
    }
}

static void Assign(wxSQLite3ResultSet &res, const wxString& column, wxString &value)
{
    for (int i=0; i<res.GetColumnCount(); i++) {
        if (res.GetColumnName(i).CmpNoCase(column)==0) {
            value = res.GetString(i);
            return;
        }
    }
}

wxString FbModelData::Format(int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) << wxChar(0xA0) << wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

//-----------------------------------------------------------------------------
//  FbTreeModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbTreeModelData, FbModelData)

//-----------------------------------------------------------------------------
//  FbTreeModelArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbTreeModelArray);

//-----------------------------------------------------------------------------
//  FbTreeStoreData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbTreeStoreData, FbTreeModelData)

size_t FbTreeStoreData::Count() const 
{ 
	return m_items.Count(); 
}

size_t FbTreeStoreData::CountAll() const 
{ 
	size_t result = 1; 
	size_t count = m_items.Count();
	for (size_t i=0; i < count; i++) 
		result += m_items.Item(i).CountAll();
	return result;
}

FbTreeModelData* FbTreeStoreData::operator[](size_t index) const
{ 
	return &m_items.Item(index); 
}

void FbTreeStoreData::Add(FbTreeModelData* data)
{
	m_items.Add(data);
}

int FbTreeStoreData::GetState(FbModel & model) const
{
	return GetState();
}

void FbTreeStoreData::SetState(FbModel & model, bool state) 
{ 
	SetState(state ? 1 : 0);
	size_t count = Count();
	for (size_t i = 0; i < count; i++) {
		m_items[i].SetState(model, state);
	}
	if (m_parent) m_parent->CheckState(model);
}

void FbTreeStoreData::CheckState(FbModel & model)
{
	int state = GetState();
	size_t count = Count();
	for (size_t i = 0; i < count; i++) {
		int substate = m_items[i].GetState(model);
		if (i == 0) SetState(substate);
		if (state != substate) {
			SetState(2);
			break;
		}
	}
	if (m_parent) m_parent->CheckState(model);
}

size_t FbTreeStoreData::GetLevel(FbModel & model) const
{ 
	return m_parent ? m_parent->GetLevel(model) + 1 : 0;
}

//-----------------------------------------------------------------------------
//  FbColumnArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbColumnArray);

//-----------------------------------------------------------------------------
//  FbModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbModel, wxObject)

FbModel::FbModel() :
    m_owner(NULL), m_position(0)
{
}

const wxBitmap & FbModel::GetBitmap(int state)
{
	if (state == wxNOT_FOUND) return wxNullBitmap;

	static wxBitmap bitmaps[3] = {
		wxBitmap(nocheck_xpm),
		wxBitmap(checked_xpm),
		wxBitmap(checkout_xpm),
	};

	return bitmaps[state % 3];
}

void FbModel::DrawItem(FbModelData &data, wxDC &dc, const wxRect &rect, const FbColumnArray &cols)
{
	if (GetCurrent() == &data) {
		dc.SetBrush(m_focused ? m_hilightBrush : m_unfocusBrush);
        dc.SetTextForeground(m_hilightColour);
	} else {
		dc.SetBrush (m_normalBrush);
		dc.SetTextForeground(m_normalColour);
	}
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetFont(data.IsBold(*this) ? m_boldFont : m_normalFont);
	dc.SetClippingRegion(rect);
	dc.DrawRectangle(rect);
	dc.SetPen(m_borderPen);
	if (m_owner->HasFlag(wxLC_HRULES)) {
		dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
	}
	dc.DestroyClippingRegion();

	int x = data.GetLevel(*this) * FB_CHECKBOX_WIDTH;
	const int y = rect.GetTop();
	const int h = rect.GetHeight();
	const wxBitmap & bitmap = GetBitmap(data.GetState(*this));

	if (data.FullRow(*this)) {
		int w = rect.GetWidth();
		wxRect rect(x, y, w, h);
		rect.Deflate(3, 2);
		wxString text = data.GetValue(*this, 0);
		dc.SetClippingRegion(rect);
		dc.DrawLabel(text, bitmap, rect);
		dc.DestroyClippingRegion();
	} else {
		size_t count = cols.Count();
		for (size_t i = 0; i < count; i++) {
			FbColumnInfo & col = cols[i];
			int w = col.GetWidth();
			if (i == 0) {
				w -= x;
			} else if (m_owner->HasFlag(wxLC_VRULES)) {
				dc.DrawLine (x, y, x, y + h);
				x++; w--;
			}
			wxRect rect(x, y, w, h);
			rect.Deflate(3, 2);
			wxString text = data.GetValue(*this, col.GetColumn());
			dc.SetClippingRegion(rect);
			dc.DrawLabel(text, i ? wxNullBitmap : bitmap, rect, col.GetAlignment());
			dc.DestroyClippingRegion();
			x += w;
		}
	}
}

void FbModel::DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	m_normalFont = dc.GetFont();
	m_normalFont.SetWeight(wxFONTWEIGHT_NORMAL);
	m_boldFont = m_normalFont;
	m_boldFont.SetWeight(wxFONTWEIGHT_BOLD);

    // Set brush colour
    m_normalBrush  = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX), wxSOLID);
    m_hilightBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID);
    m_unfocusBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), wxSOLID);
    // Set font colour
    m_normalColour  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	m_hilightColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    // Set pen for borders
	m_borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);

	DoDrawTree(dc, rect, cols, pos, h);
}

//-----------------------------------------------------------------------------
//  FbListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbListModel, FbModel)

void FbListModel::DoDrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	for ( ; pos < count && y < yy; pos++, y+=h )
	{
		wxRect rect(0, y, ww, h);
        FbModelData * data = GetData(pos + 1);
		if (data) DrawItem(*data, dc, rect, cols);
	}
}

int FbListModel::GoFirstRow()
{
	size_t count = GetRowCount();

	if (count) {
		m_position = 1;
	    return m_position;
    } else {
    	m_position = 0;
        return wxNOT_FOUND;
    }
}

int FbListModel::GoLastRow()
{
	size_t count = GetRowCount();

	if (count) {
		m_position = count;
	    return m_position;
    } else {
		m_position = 0;
        return wxNOT_FOUND;
    }
}

int FbListModel::GoNextRow(size_t delta)
{
 	size_t count = GetRowCount();
	if (count) {
		m_position = m_position + delta <= count ? m_position + delta : count;
		return m_position;
	} else 
		return 0;
}

int FbListModel::GoPriorRow(size_t delta)
{
 	size_t count = GetRowCount();
	if (count) {
		m_position = m_position <= delta ? 1 : m_position - delta;
		return m_position;
	} else 
		return 0;
}

size_t FbListModel::FindRow(size_t row, bool select)
{
 	size_t count = GetRowCount();
	if (count && row && row <= count ) {
		if (select) m_position = row;
		return row;
	} else 
		return m_position;
}

//-----------------------------------------------------------------------------
//  FbListStore
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbListStore, FbListModel)

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbModelDataArray)

void FbListStore::Append(FbModelData * data)
{ 
	m_list.Add(data); 
	size_t count = m_list.Count();
	if (count == 1) m_position = 1;
	if (m_owner) {
		m_position = count;
		m_owner->Refresh();
	}
}

void FbListStore::Replace(FbModelData * data)
{
	size_t count = m_list.Count();
	if (m_position && m_position <= count) {
		m_list.RemoveAt(m_position - 1); 
		m_list.Insert(data, m_position - 1); 
	}
	if (m_owner) m_owner->Refresh();
}

void FbListStore::Delete()
{
	size_t count = m_list.Count();
	if (m_position && m_position<=count) {
		m_list.RemoveAt(m_position - 1); 
		if (m_position >= count) m_position = count - 1;
		if (m_owner) m_owner->Refresh();
	}
}

/*
//-----------------------------------------------------------------------------
//  FbListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbListModel, FbModel)

FbListModel::FbListModel(size_t count):
	m_count(count)
{
	if (count) {
		m_current = FbItemKeyList(1);
		m_position = 1;
	}
}

FbListModel::~FbListModel()
{
}

FbListModelData * FbListModel::FindRow(const FbItemId &id)
{
	size_t row = 0;
	return FbItemKeyList::Key(id, row) ? FindRow(row, false) : NULL;
}

FbListModelData * FbListModel::FindRow(size_t rowid, bool select)
{
	FbListModelData * data = NULL;
    for (size_t i=0; i < m_data.Count(); i++) {
        if (m_data[i].GetRow() == rowid) {
			data = m_data.Detach(i);
			m_data.Insert(data, 0);
            break;
        }
    }

	if (data == NULL) {
		data = ReadData(rowid);
		if (data) m_data.Insert(data, 0);

		if (m_data.Count() > TREELIST_CASHE_SIZE)
			m_data.RemoveAt(TREELIST_CASHE_SIZE, m_data.Count() - TREELIST_CASHE_SIZE);
	}

	if (select) {
		m_current = data->GetId();
		m_position = rowid;
	}

	return data;
}

void FbListModel::SetRowCount(size_t count)
{
	m_count = count;
	if (m_count && !m_current.IsOk()) m_current = FbItemKeyList(1);
}

FbItemId FbListModel::FindItem(size_t row, bool select)
{
	if (row && row<=GetRowCount()) {
		FbItemId res = FbItemKeyList(row);
		if (select) {
			m_position = row;
			m_current = res;
		}
		return res;
	}
	return FbItemId();
}

//-----------------------------------------------------------------------------
//  FbTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbTreeModel, FbModel)

void FbTreeModel::DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	m_normalFont = dc.GetFont();
	m_normalFont.SetWeight(wxFONTWEIGHT_NORMAL);
	m_boldFont = m_normalFont;
	m_boldFont.SetWeight(wxFONTWEIGHT_BOLD);

	if (m_root) {
		int y = 0;
		m_root->Draw(*this, dc, rect, cols, h, y);
	}
}

int FbTreeModel::GoFirstRow()
{
	if (m_root) {
		m_position = 1;
		m_current = m_root->GetId();
		return 1;
	} else {
		m_position = 0;
		m_current = FbItemId();
		return 0;
	}
}

int FbTreeModel::GoLastRow()
{
	if (m_root) {
		m_position = m_root->RowCount();
		m_current = m_root->GetLast();
		return m_position;
	} else {
		m_position = 0;
		m_current = FbItemId();
		return 0;
	}
}

int FbTreeModel::GoPriorRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count) {
		m_position = m_position > delta ? m_position - delta : 1;
		FindItem(m_position, true);
		return m_position;
	} else {
		m_position = 0;
		m_current = FbItemId();
		return 0;
	}
}

int FbTreeModel::GoNextRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count) {
		m_position = m_position + delta < count ? m_position + delta : count;
		FindItem(m_position, true);
		return m_position;
	} else {
		m_position = 0;
		m_current = FbItemId();
		return 0;
	}
}

//-----------------------------------------------------------------------------
//  FbTreeModelData
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbTreeModelArray)

IMPLEMENT_CLASS(FbTreeModelData, FbModelData)

FbTreeModelData::FbTreeModelData(const FbItemId &id, wxSQLite3ResultSet &result)
	: FbModelData(result.GetString(1)), m_rowid(id)
{
}

void FbTreeModelData::Draw(FbModel &model, wxDC &dc, const wxRect &rect, const FbColumnArray &cols, int h, int &y)
{
	if (y > rect.GetBottom()) return;

	int ww = rect.GetWidth();

	if (y >= rect.GetTop()) {
		wxRect rect(0, y, ww, h);
		model.DrawItem(*this, dc, rect, cols);
	}
	y += h;

	size_t count = Count();
	for (size_t i = 0; i < count; i++) {
		Items(model, i)->Draw(model, dc, rect, cols, h, y);
	}
}

*/

