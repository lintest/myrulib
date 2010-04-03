#include "FbTreeModel.h"

#include "../MyRuLib/FbLogoBitmap.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbColumnArray);

const wxBitmap & FbTreeModel::GetBitmap(int state) const
{
	static wxBitmap bitmaps[3] = {
		wxBitmap(nocheck_xpm),
		wxBitmap(checked_xpm),
		wxBitmap(checkout_xpm),
	};

	return bitmaps[state % 3];
}

void FbTreeModelList::Draw(wxDC &dc, const wxRect &rect, const FbColumnArray &cols, size_t pos, int h)
{
	int ww = rect.GetWidth();
	int y  = rect.GetTop();
	int yy = rect.GetBottom();
	size_t count = GetRowCount();

	for ( ; pos < count && y < yy; pos++, y+=h )
	{
        wxString text = wxString::Format(_("Paint %d"), pos);
        for (size_t j=0; j<pos%30; j++) text += _(" 0");

		wxRect rect(0, y, ww, h);
        rect.Deflate(2, 2);
		dc.DrawLabel(text, GetBitmap(pos), rect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	}
}

FbTreeItemId FbTreeModelList::GetFirst()
{
	size_t count = GetRowCount();
	FbTreeItemKey * key = NULL;
	if (count) key = new FbTreeItemKeyList(0); ;
	return FbTreeItemId(key);
}

FbTreeItemId FbTreeModelList::GetLast()
{
	size_t count = GetRowCount();
	FbTreeItemKey * key = NULL;
	if (count) key = new FbTreeItemKeyList(count - 1);
	return FbTreeItemId(key);
}

FbTreeItemId FbTreeModelList::GetNext(const FbTreeItemId &id)
{
	if (id.GetKeyType() == FbTreeItemKey::KT_LIST) {
		size_t rowid = ((FbTreeItemKeyList*)id.GetKey())->GetId();
		size_t count = GetRowCount();
		return rowid < count ? FbTreeItemId(&FbTreeItemKeyList(rowid + 1)) : id;
	}
	return GetFirst();
}

FbTreeItemId FbTreeModelList::GetPrior(const FbTreeItemId &id)
{
	if (id.GetKeyType() == FbTreeItemKey::KT_LIST) {
		size_t rowid = ((FbTreeItemKeyList*)id.GetKey())->GetId();
		size_t count = GetRowCount();
		return rowid > 0 ? FbTreeItemId(&FbTreeItemKeyList(rowid - 1)) : id;
	}
	return GetFirst();
}

