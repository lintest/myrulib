#include "FbTreeModel.h"
#include "FbTreeView.h"
#include <wx/listbase.h>
#include <wx/renderer.h>

#include "FbLogoBitmap.h"

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbColumnInfo, wxObject)

//-----------------------------------------------------------------------------
//  FbModelItem
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbModelItem, wxObject)

FbModelItem & FbModelItem::operator =(const FbModelItem &item)
{
	if (m_virual) wxDELETE(m_data);
	m_model = item.m_model;
	if (item && item.m_virual) {
		m_data = item.m_data->Clone();
	} else {
		m_data = item.m_data;
	}
	m_virual = item.m_virual;
	return *this;
}

int FbModelItem::Level() 
{ 
	int level = 0;
	FbModelItem parent = GetParent();
	while (parent) {
		parent = parent.GetParent();
		if (parent) level++;
	}
	return level;
}

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbModelData, wxObject)

wxString FbModelData::Format(int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) << wxChar(0xA0) << wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

int FbModelData::GetState(FbModel & model) const
{
	return DoGetState(model);
}

void FbModelData::SetState(FbModel & model, bool state)
{
	int old_state = DoGetState(model);
	if (state) {
		if (old_state == 1) return;
	} else {
		if (old_state == 0) return;
	}

	size_t count = Count(model);
	for (size_t i = 0; i < count; i++) {
		Items(model, i)->SetState(model, state);
	}
	DoSetState(model, state ? 1 : 0);
	FbModelData * parent = GetParent(model);
	if (parent) parent->CheckState(model);
}

void FbModelData::CheckState(FbModel & model)
{
	int state = GetState(model);
	size_t count = Count(model);
	for (size_t i = 0; i < count; i++) {
		int substate = Items(model, i)->GetState(model);
		if (i == 0) state = substate;
		if (state != substate) {
			state = 2;
			break;
		}
	}
	DoSetState(model, state);
	FbModelData * parent = GetParent(model);
	if (parent) parent->CheckState(model);
}

//-----------------------------------------------------------------------------
//  FbParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbParentData, FbModelData)

FbParentData::FbParentData(FbModel & model, FbParentData * parent)
	: m_parent(parent)
{
	if (m_parent) m_parent->Add(model, this);
}

size_t FbParentData::CountAll(const FbModel & model) const
{
	size_t result = 1;
	size_t count = m_items.Count();
	for (size_t i=0; i < count; i++)
		result += m_items.Item(i).CountAll(model);
	return result;
}

FbModelData* FbParentData::Items(FbModel & model, size_t index) const
{
	return &m_items.Item(index);
}

void FbParentData::Add(FbModel & model, FbModelData* data)
{
	if (data == NULL) return;
	int state = GetState(model);
	if (state == 2) data->SetState(model, 0);
	else data->SetState(model, state);
	m_items.Add(data);
}

//-----------------------------------------------------------------------------
//  FbChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbChildData, FbModelData)

FbChildData::FbChildData(FbModel & model, FbParentData * parent)
	: m_parent(parent)
{
	if (m_parent) m_parent->Add(model, this);
}

//-----------------------------------------------------------------------------
//  FbColumnArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbColumnArray);

//-----------------------------------------------------------------------------
//  FbModel::PaintContext
//-----------------------------------------------------------------------------

static wxColour MiddleColour(wxColour text, wxColour back)
{
	unsigned char r = text.Red() / 2 + text.Red() / 2;
	unsigned char g = text.Green() / 2 + text.Green() / 2;
	unsigned char b = text.Blue() / 2 + text.Blue() / 2;
	return wxColour(r, g, b);

}

FbModel::PaintContext::PaintContext(FbModel &model, wxDC &dc):
	m_window(model.GetOwner()), 
	// Set brush colour
	m_normalBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX), wxSOLID),
	m_hilightBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID),
	m_unfocusBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), wxSOLID),
	// Set font colour
	m_normalColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)),
	m_hilightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT)),
	m_graytextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT)),
	// Set pen for borders
	m_borderPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID),
	m_directory(model.GetOwner()->HasFlag(fbTR_DIRECTORY)),
	// Current item flags
	m_current(false),
	m_selected(false),
	m_multuply(model.GetOwner()->HasFlag(fbTR_MULTIPLE)),
	m_checkbox(model.GetOwner()->HasFlag(fbTR_CHECKBOX)),
	m_vrules(model.GetOwner()->HasFlag(fbTR_VRULES)),
	m_hrules(model.GetOwner()->HasFlag(fbTR_HRULES)),
	m_level(0)
{
	if (m_graytextColour == m_normalColour)
		MiddleColour(m_normalColour, wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));

	m_normalFont = dc.GetFont();
	m_normalFont.SetWeight(wxFONTWEIGHT_NORMAL);
	m_boldFont = m_normalFont;
	m_boldFont.SetWeight(wxFONTWEIGHT_BOLD);
}

//-----------------------------------------------------------------------------
//  FbModel
//-----------------------------------------------------------------------------

int CompareSizeT(size_t x, size_t y)
{
	return x - y;
}

IMPLEMENT_CLASS(FbModel, wxObject)

FbModel::FbModel() :
	m_owner(NULL), m_position(0), m_shift(0), m_ctrls(CompareSizeT)
{
}

const wxBitmap & FbModel::GetBitmap(int state)
{
	static wxBitmap bitmaps[3] = {
		wxBitmap(nocheck_xpm),
		wxBitmap(checked_xpm),
		wxBitmap(checkout_xpm),
	};

	return bitmaps[state % 3];
}

void FbModel::DrawButton(const FbModelItem &data, wxWindow * window, wxDC &dc, wxRect &rect)
{
	int h = rect.height; if (h % 2 == 0) h--;
	wxRect r(rect.x, rect.y, h, h);
	rect.x += h + 2;
	if (!data.HasChildren()) return;
	r.Deflate(1);
    int flag = data.IsExpanded() ? wxCONTROL_EXPANDED: 0;
	wxRendererNative::GetDefault().DrawTreeItemButton(window, dc, r, flag);
}

void FbModel::DrawItem(FbModelItem &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols)
{
	if (ctx.m_selected) {
		dc.SetBrush(m_focused ? ctx.m_hilightBrush : ctx.m_unfocusBrush);
		dc.SetTextForeground(ctx.m_hilightColour);
	} else {
		dc.SetBrush (ctx.m_normalBrush);
		dc.SetTextForeground(data.IsGray() ? ctx.m_graytextColour : ctx.m_normalColour);
	}
	if (ctx.m_current) {
		dc.SetPen(ctx.m_borderPen);
	} else {
		dc.SetPen(*wxTRANSPARENT_PEN);
	}
	dc.SetFont(data.IsBold() ? ctx.m_boldFont : ctx.m_normalFont);
	dc.SetClippingRegion(rect);
	dc.DrawRectangle(rect);
	dc.SetPen(ctx.m_borderPen);
	if (ctx.m_hrules) dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
	dc.DestroyClippingRegion();

	int x = ctx.m_level * FB_CHECKBOX_WIDTH;
	const int y = rect.GetTop();
	const int h = rect.GetHeight();
	const wxBitmap & bitmap = ctx.m_checkbox ? GetBitmap(data.GetState()) : wxNullBitmap;

	if (data.FullRow()) {
		int w = rect.GetWidth();
		wxRect rect(x, y, w, h);
		rect.Deflate(3, 2);
		wxString text = data[0];
		dc.SetClippingRegion(rect);
		if (ctx.m_directory) DrawButton(data, ctx.m_window, dc, rect);
		dc.DrawLabel(text, bitmap, rect, wxALIGN_CENTRE_VERTICAL);
		dc.DestroyClippingRegion();
	} else {
		size_t count = cols.Count();
		for (size_t i = 0; i < count; i++) {
			FbColumnInfo & col = cols[i];
			int w = col.GetWidth();
			if (i == 0) {
				w -= x;
			} else if (ctx.m_vrules) {
				dc.DrawLine (x, y, x, y + h);
				x++; w--;
			}
			wxRect rect(x, y, w, h);
			rect.Deflate(3, 2);
			wxString text = data[col.GetColumn()];
			dc.SetClippingRegion(rect);
			if (ctx.m_directory && i == 0) DrawButton(data, ctx.m_window, dc, rect);
			dc.DrawLabel(text, i ? wxNullBitmap : bitmap, rect, col.GetAlignment() | wxALIGN_CENTRE_VERTICAL);
			dc.DestroyClippingRegion();
			x += w;
		}
	}
}

void FbModel::DrawTree(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	PaintContext ctx(*this, dc);
	DoDrawTree(dc, ctx, rect, cols, pos, h);
}

bool FbModel::IsSelected(size_t row)
{
	if (m_shift) {
		return (m_shift <= row && row <= m_position) || (m_position <= row && row <= m_shift);
	} else {
		if (m_ctrls.Count() == 0)
			return m_position == row;
		else
			return m_ctrls.Index(row) != wxNOT_FOUND;
	}
}

void FbModel::SetShift(bool select)
{
	m_shift  = select ? ( m_shift ? m_shift : m_position ) : 0;
	m_ctrls.Empty();
}

void FbModel::InitCtrls()
{
	if (m_position == 0 || m_ctrls.Count() > 0) return;
	size_t min  = m_shift < m_position ? (m_shift ? m_shift : m_position) : m_position;
	size_t max = m_shift > m_position ? m_shift : m_position;
	for (size_t i = min; i <= max; i++) m_ctrls.Add(i);
	m_shift = 0;
}

void FbModel::InvertCtrl()
{
	if (m_position == 0) return;

	int index = m_ctrls.Index(m_position);
	if (index == wxNOT_FOUND) {
		m_ctrls.Add(m_position);
	} else {
		m_ctrls.RemoveAt(index);
	}
}

void FbModel::SingleCheck(size_t row)
{
	if (row == 0) row = GetPosition();
	FbModelItem item = GetData(row);
	if (item) {
		int state = item.GetState() == 1 ? 0 : 1;
		item.SetState(state);
	}
}

//-----------------------------------------------------------------------------
//  FbListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbListModel, FbModel)

void FbListModel::DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	size_t row = pos;
	for ( ; row < count && y < yy; y+=h )
	{
		row++;
		wxRect rect(0, y, ww, h);
		FbModelItem item = GetData(row);
		if (ctx.m_multuply) {
			ctx.m_current = m_position == row;
			ctx.m_selected = IsSelected(row);
		} else {
			ctx.m_current = false;
			ctx.m_selected = m_position == row;
		}
		if (item) DrawItem(item, dc, ctx, rect, cols);
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

void FbListModel::MultiplyCheck()
{
	if (m_position == 0) return;

	if (m_shift > 0)  {
		size_t min = m_shift < m_position ? m_shift : m_position;
		size_t max = m_shift > m_position ? m_shift : m_position;
		FbModelItem item = GetData(min);
		if (!item) return;
		int state = item.GetState() == 1 ? 0 : 1;
		for (size_t i = min; i <= max; i++ ) {
			FbModelItem item = GetData(i);
			if (item) item.SetState(state);
		}
	} else {
		size_t count = m_ctrls.Count();
		if (count) {
			FbModelItem item = GetData(m_ctrls[0]);
			if (!item) return;
			int state = item.GetState() == 1 ? 0 : 1;
			for (size_t i = 0; i < count; i++ ) {
				FbModelItem item = GetData(m_ctrls[i]);
				if (item) item.SetState(state);
			}
		} else {
			SingleCheck(m_position);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbListStore
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbListStore, FbListModel)

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbModelDataArray)

void FbListStore::Insert(FbModelData * data, size_t pos)
{
	m_list.Insert(data, pos);
	size_t count = m_list.Count();

	if (count == 1) {
		m_position = 1; 
	} else {
		if (m_position >= pos) m_position++;
	}

	if (m_owner) {
		m_position = count;
		m_owner->Refresh();
	}
}

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
	if (m_position && m_position <= count) {
		m_list.RemoveAt(m_position - 1);
		if (m_position >= count) m_position = count - 1;
		if (m_owner) m_owner->Refresh();
	}
}

//-----------------------------------------------------------------------------
//  FbTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbTreeModel, FbModel)

void FbTreeModel::DoDrawTree(wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	size_t row = 0;
	if (m_root) {
		ctx.m_hidden = m_root->HiddenRoot();
		ctx.m_level = ctx.m_hidden ? -1 : 0;
		FbModelItem item(*this, m_root);
		DrawTreeItem(item, dc, ctx, rect, cols, h, row);
	}
}

void FbTreeModel::DrawTreeItem(FbModelItem &data, wxDC &dc, PaintContext &ctx, const wxRect &rect, const FbColumnArray &cols, int h, size_t &row)
{
	int y = row * h;
	if (y > rect.GetBottom()) return;

	int ww = rect.GetWidth();

	if (ctx.m_hidden) {
		ctx.m_hidden = false;
	} else {
		row++;
		if (y >= rect.GetTop()) {
			ctx.m_current = m_position == row;
			if (ctx.m_multuply) {
				ctx.m_selected = IsSelected(row);
			} else {
				ctx.m_selected = m_position == row;
			}
			wxRect rect(0, y, ww, h);
			DrawItem(data, dc, ctx, rect, cols);
		}
	}

	ctx.m_level++;
	size_t count = data.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = data.Items(i);
		if (child) DrawTreeItem(child, dc, ctx, rect, cols, h, row);
	}
	ctx.m_level--;
}

int FbTreeModel::GoFirstRow()
{
	if (m_root) {
		return m_position = 1;
	} else {
		return m_position = 0;
	}
}

int FbTreeModel::GoLastRow()
{
	if (m_root) {
		return m_position = GetRowCount();
	} else {
		return m_position = 0;
	}
}

FbModelItem FbTreeModel::GetLast(FbModelItem &parent)
{
	size_t count = parent.Count();
	if (count) {
		FbModelItem child = parent.Items(count-1);
		if (child) return GetLast(child);
	}
	return parent;
}

int FbTreeModel::GoPriorRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count) {
		m_position = m_position > delta ? m_position - delta : 1;
		FindRow(m_position, true);
		return m_position;
	} else {
		m_position = 0;
		return 0;
	}
}

int FbTreeModel::GoNextRow(size_t delta)
{
	size_t count = GetRowCount();
	if (count) {
		m_position = m_position + delta < count ? m_position + delta : count;
		FindRow(m_position, true);
		return m_position;
	} else {
		m_position = 0;
		return 0;
	}
}

size_t FbTreeModel::FindRow(size_t row, bool select)
{
	if (m_root) {
		int level = 0;
		size_t pos = row;
		FbModelItem root(*this, m_root);
		FbModelItem item = FindData(root, pos, level);
		if (item) {
			if (select) m_position = row;
			return row;
		}
	}
	return 0;
}

FbModelItem FbTreeModel::FindData(FbModelItem &parent, size_t &row, int &level)
{
	row--;
	if (row == 0) { return parent; }
	level++;
	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = parent.Items(i);
		FbModelItem result = FindData(child, row, level);
		if (result) return result;
	}
	level--;
	return *this;
}

FbModelItem FbTreeModel::DoGetData(size_t row, int &level)
{
	if (m_root) {
		size_t pos = row;
		if (m_root->HiddenRoot()) pos++;
		level = m_root->HiddenRoot() ? -1 : 0;
		FbModelItem item(*this, m_root);
		return FindData(item, pos, level);
	}
	return *this;
}

void FbTreeModel::SetRoot(FbModelData * root)
{
	wxDELETE(m_root);
	m_root = root;
	m_position = 1;
}

size_t FbTreeModel::GetRowCount() const
{
	if (m_root) {
		size_t count = m_root->CountAll(*this);
		if (m_root->HiddenRoot()) count--;
		return count;
	}
	return 0;
}

void FbTreeModel::Delete()
{
	if (m_root == NULL) return;
	if (m_position == 0) return;

	size_t row = m_position - 1;
	if (m_root->HiddenRoot()) row++;
	DoDelete(*m_root, row);
}

bool FbTreeModel::DoDelete(FbModelData &parent, size_t &row)
{
	size_t count = parent.Count(*this);
	for (size_t i = 0; i < count; i++) {
		row--;
		if (row == 0) {
			FbParentData * data = wxDynamicCast(&parent, FbParentData);
			if (data) {
				data->Delete(i);
				if (i == count-1) m_position--;
				return true;
			} else return false;
		} else {
			bool ok = DoDelete(*parent.Items(*this, i), row);
			if (ok) return true;
		}
	}
	return false;
}

void FbTreeModel::MultiplyCheck()
{
	if (m_root == NULL) return;
	if (m_position == 0) return;

	size_t max;
	if (m_shift) {
		max = m_shift > m_position ? m_shift : m_position;
	} else {
		size_t count = m_ctrls.Count();
		max = count ? m_ctrls[count - 1] : m_position;
	}

	int state = wxNOT_FOUND;
	size_t row = m_root->HiddenRoot() ? 0 : 1;
	FbModelItem item(*this, m_root);
	DoCheck(item, max, row, state);
}

void FbTreeModel::DoCheck(FbModelItem &parent, size_t max, size_t &row, int &state)
{
	if (IsSelected(row++)) {
		if (state == wxNOT_FOUND) state = parent.GetState() == 1 ? 0 : 1;
		parent.SetState(state);
	}
	size_t count = parent.Count();
	for (size_t i = 0; i < count && row <= max; i++) {
		FbModelItem child = parent.Items(i);
		DoCheck(child, max, row, state);
	}
}

void FbTreeModel::SelectAll(bool value)
{
	if (m_root) {
		FbModelItem item(*this, m_root);
		DoSelect(item, value);
	}
}

void FbTreeModel::DoSelect(FbModelItem &parent, bool value)
{
	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = parent.Items(i);
		child.SetState(value);
		DoSelect(child, value);
	}
}
