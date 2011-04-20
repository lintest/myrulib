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

int FbAuthParentData::Compare(const FbAuthParentData &data) const
{
	return wxStrcoll(GetTitle(), GetTitle());
}

//-----------------------------------------------------------------------------
//  FbSeqnParentData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnParentData, FbParentData)

int FbSeqnParentData::Compare(const FbSeqnParentData &data) const
{
	if (GetCode() == 0) return +1;
	if (data.GetCode() == 0) return -1;
	return wxStrcoll(GetTitle(), GetTitle());
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

void FbBookTreeModel::DoTraverse(FbBookTraverser & traverser)
{
	FbModelItem root = GetRoot();
	if (!root) return;

	size_t count = GetChecked(traverser, root, 0);
	if (count) return;

	size_t pos = GetPosition();
	if (pos == 0) return;

	size_t max;
	if (m_shift) {
		max = m_shift > m_position ? m_shift : m_position;
	} else {
		size_t count = m_ctrls.Count();
		max = count ? m_ctrls[count - 1] : m_position;
	}

	size_t row = m_root->HiddenRoot() ? 0 : 1;
	GetSelected(traverser, root, 0, max, row);
}

size_t FbBookTreeModel::GetChecked(FbBookTraverser & traverser, FbModelItem &parent, int level)
{
	size_t result = 0;

	if (parent.GetState() > 0) {
		traverser.OnBook(parent, level);
		result++;
	}

	size_t count = parent.Count();
	for (size_t i = 0; i < count; i++) {
		FbModelItem child = parent.Items(i);
		result += GetChecked(traverser, child, level + 1);
	}

	return result;
}

size_t FbBookTreeModel::GetSelected(FbBookTraverser & traverser, FbModelItem &parent, int level, size_t max, size_t &row)
{
	size_t result = 0;

	if (IsSelected(row++)) {
		traverser.OnBook(parent, level);
		result++;
	}

	size_t count = parent.Count();
	for (size_t i = 0; i < count && row <= max; i++) {
		FbModelItem child = parent.Items(i);
		result += GetSelected(traverser, child, level + 1, max, row);
	}

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
	if (m_root == NULL) return;
	FbModelItem item(*this, m_root);
	wxArrayInt items;
	GetSelected(items);
	if (items.Count() == 0) MultiplyCheck();
	size_t row = GetRowCount();
	DoDelete(item, row);
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
