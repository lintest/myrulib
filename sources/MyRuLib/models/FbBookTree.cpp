#include "FbBookTree.h"
#include "FbBookEvent.h"
#include "FbDatabase.h"
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

int FbAuthParentData::Compare(const FbAuthParentData &data) const
{
	return ::FbCompare(GetTitle(), data.GetTitle());
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

int FbSeqnParentData::Compare(const FbSeqnParentData &data) const
{
	if (GetCode() == 0) return +1;
	if (data.GetCode() == 0) return -1;
	return FbCompare(GetTitle(), data.GetTitle());
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
	FbBookArrayTraverser traverser(items);
	DoTraverse(traverser);
	return items.Count();
}

wxString FbBookTreeModel::GetText(wxArrayInt &columns)
{
	FbBookTextTraverser traverser(columns);
	DoTraverse(traverser);
	return traverser.GetText();
}

size_t FbBookTreeModel::DoTraverse(FbBookTraverser & traverser)
{
	FbModelItem root = GetRoot();
	if (!root) return 0;

	size_t row, max = GetRowCount();

	if (size_t res = GetChecked(traverser, root, 0, row = max)) return res;

	return GetSelected(traverser, root, 0, row = max);
}

size_t FbBookTreeModel::GetChecked(FbBookTraverser & traverser, FbModelItem &parent, int level, size_t &row)
{
	size_t result = 0;
	size_t count = parent.Count();
	for (size_t i = count; i > 0; i--) {
		FbModelItem child = parent.Items(i - 1);
		result += GetChecked(traverser, child, level + 1, row);
	}
	if (parent.GetState() > 0) {
		traverser.OnBook(parent, level, row);
		result++;
	}

	row--;

	return result;
}

size_t FbBookTreeModel::GetSelected(FbBookTraverser & traverser, FbModelItem &parent, int level, size_t &row)
{
	size_t result = 0;

	size_t count = parent.Count();
	for (size_t i = count; i > 0; i--) {
		FbModelItem child = parent.Items(i - 1);
		result += GetSelected(traverser, child, level + 1, row);
	}

	if (IsSelected(row)) {
		traverser.OnBook(parent, level, row);
		result++;
	}

	row--;

	return result;
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

void FbBookTreeModel::Delete()
{
	FbModelItem root = GetRoot();
	if (!root) return;

	size_t row = 0;
	wxArrayInt items;
	FbBookArrayTraverser traverser(items);
	DoTraverse(traverser);
	size_t count = GetChecked(traverser, root, 0, row);
	if (!count) MultiplyCheck();

	row = GetRowCount();
	DoDelete(root, row);
	m_ctrls.Empty();
	m_shift = 0;
}

bool FbBookTreeModel::DoDelete(FbModelItem &parent, size_t &row)
{
	FbParentData * data = wxDynamicCast(&parent, FbParentData);
	if (data == NULL) return false;

	bool remove = false;
	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		size_t index = count - i - 1;
		FbModelItem child = parent.Items(index);
		remove = DoDelete(child, row) || remove;
		if (child.GetState() != 0 && child.Count() == 0) {
			data->Delete(index);
			if (m_position >= row) m_position--;
			remove = true;
		} else (&child)->CheckState(*this);
		row--;
	}
	return remove;
}

int FbBookTreeModel::GetBookCount()
{
	FbModelItem root = GetRoot();
	if (!root) return 0;

	FbSortedArrayInt books(FbArrayEvent::CompareInt);
	GetBooks(root, books);
	return books.Count();
}

void FbBookTreeModel::GetBooks(FbModelItem &parent, FbSortedArrayInt &items)
{
	FbBookChildData * book = wxDynamicCast(&parent, FbBookChildData);
	if (book && items.Index(book->GetCode()) == wxNOT_FOUND) items.Add(book->GetCode());

	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = parent.Items(i);
		GetBooks(child, items);
	}
}
