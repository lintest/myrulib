#include "FbBookTree.h"
#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbAuthParentData
//-----------------------------------------------------------------------------

static int ComareBooks(FbModelData ** x, FbModelData ** y)
{
	{
		FbSeqnParentData * xx = wxDynamicCast(*x, FbSeqnParentData);
		FbSeqnParentData * yy = wxDynamicCast(*y, FbSeqnParentData);
		if (xx && yy) return xx->Compare(*yy);
	}
	{
		FbAuthParentData * xx = wxDynamicCast(*x, FbAuthParentData);
		FbAuthParentData * yy = wxDynamicCast(*y, FbAuthParentData);
		if (xx && yy) return xx->Compare(*yy);
	}

	return 0;
}

IMPLEMENT_CLASS(FbAuthParentData, FbParentData)

void FbAuthParentData::SortItems()
{
	m_items.Sort(ComareBooks);
}

int FbAuthParentData::Compare(const FbAuthParentData &data)
{
	return GetTitle().CmpNoCase(data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

int FbSeqnParentData::Compare(const FbSeqnParentData &data)
{
	if (GetCode() == 0) return +1;
	if (data.GetCode() == 0) return -1;
	return GetTitle().CmpNoCase(data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbBookChildData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookChildData, FbChildData)

wxString FbBookChildData::GetValue(FbModel & model, size_t col) const
{
	if (col == BF_NUMB)
		return m_numb ? wxString::Format(wxT("%d"), m_numb) : wxString();
	else
		return FbCollection::GetBook(m_code, col);
}

//-----------------------------------------------------------------------------
//  FbBookTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookTreeModel, FbTreeModel)

size_t FbBookTreeModel::GetSelected(wxArrayInt &items)
{
	items.Empty();
	FbModelItem root = GetRoot();
	if (!root) return 0;

	GetChecked(root, items);
	size_t count = items.Count();
	if (count) return count;

	size_t pos = GetPosition();
	if (pos == 0) return 0;

	size_t max;
	if (m_shift) {
		max = m_shift > m_position ? m_shift : m_position;
	} else {
		size_t count = m_ctrls.Count();
		max = count ? m_ctrls[count - 1] : m_position;
	}

	size_t row = m_root->HiddenRoot() ? 0 : 1;
	GetSelected(root, max, row, items);
	return items.Count();
}

void FbBookTreeModel::GetChecked(FbModelItem &parent, wxArrayInt &items)
{
	if (parent.GetState() == 1) {
		FbBookChildData * book = wxDynamicCast(&parent, FbBookChildData);
		if (book) items.Add(book->GetCode());
	}

	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = parent.Items(i);
		GetChecked(child, items);
	}
}

void FbBookTreeModel::GetSelected(FbModelItem &parent, size_t max, size_t &row, wxArrayInt &items)
{
	if (IsSelected(row++)) {
		FbBookChildData * book = wxDynamicCast(&parent, FbBookChildData);
		if (book) items.Add(book->GetCode());
	}
	size_t count = parent.Count();
	for (size_t i = 0; i < count && row <= max; i++) {
		FbModelItem child = parent.Items(i);
		GetSelected(child, max, row, items);
	}
}

int FbBookTreeModel::GetBook()
{
	FbModelItem item = GetCurrent();
	FbBookChildData * book = wxDynamicCast(&item, FbBookChildData);
	return book ? book->GetCode() : 0;
}

FbViewItem FbBookTreeModel::GetView()
{
	FbModelItem item = GetCurrent();
	return item ? (&item)->GetView() : FbViewItem::None;
}
