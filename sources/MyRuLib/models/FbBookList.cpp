#include "FbBookList.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbBookListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookListData, FbModelData)

void FbBookListData::DoSetState(FbModel & model, int state)
{
	FbBookListModel * booklist = wxDynamicCast(&model, FbBookListModel);
	if (booklist) booklist->SetState(m_code, state);
}

int FbBookListData::DoGetState(FbModel & model) const
{
	FbBookListModel * booklist = wxDynamicCast(&model, FbBookListModel);
	return booklist ? booklist->GetState(m_code) : 0;
}

//-----------------------------------------------------------------------------
//  FbBookListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbBookListModel, FbListModel)

FbBookListModel::FbBookListModel(const wxArrayInt &items)
	: m_check(CompareInt)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
}

FbBookListModel::~FbBookListModel(void)
{
}

void FbBookListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

void FbBookListModel::SetState(int code, int state)
{
	int index = m_check.Index(code);
	if (index == wxNOT_FOUND) {
		if (state) m_check.Add(code);
	} else {
		if (!state) m_check.RemoveAt(index);
	}
}

int FbBookListModel::GetState(int code) const
{
	return m_check.Index(code) != wxNOT_FOUND;
}

FbModelItem FbBookListModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_items.Count()) return *this;
	int code = m_items[row - 1];
	FbBookListData data(code);
	return FbModelItem(*this, &data);
}

void FbBookListModel::SelectAll(bool value)
{
	m_check.Empty();
	if (value) {
		size_t count = m_items.Count();
		m_check.Alloc(count);
		for (size_t i = 0; i < count; i++) {
			m_check.Add(m_items[i]);
		}
	}
}

size_t FbBookListModel::GetSelected(wxArrayInt &items)
{
	items.Empty();
	FbBookArrayTraverser traverser(items);
	DoTraverse(traverser);
	return items.Count();
}

wxString FbBookListModel::GetText(wxArrayInt &columns)
{
	FbBookTextTraverser traverser(columns);
	DoTraverse(traverser);
	return traverser.GetText();
}

FbModelItem FbBookListModel::Items(size_t index)
{
	FbBookListData data = m_items[index];
	return FbModelItem(*this, &data);
}

size_t FbBookListModel::DoTraverse(FbBookTraverser & traverser)
{
	size_t count = m_items.Count();
	if (count == 0) return 0;

	size_t check_count = 0;
	for (size_t row = count; row > 0; row--) {
		int book = m_items[row - 1];
		if (m_check.Index(book) == wxNOT_FOUND) continue;
		FbBookListData data = book;
		FbModelItem item(*this, &data);
		traverser.OnBook(item, 0, row);
		check_count++;
	}
	if (check_count) return check_count;

	if (m_position == 0) return 0;

	if (m_shift) {
		size_t min = m_shift < m_position ? m_shift : m_position;
		size_t max = m_shift > m_position ? m_shift : m_position;
		for (size_t row = max; row >= min; row--) {
			traverser.OnBook(Items(row - 1), 0, row);
		}
		return max - min + 1;
	} else {
		if (m_ctrls.Count() == 0) {
			traverser.OnBook(Items(m_position - 1), 0, m_position);
			return 1;
		} else {
			size_t check_count = 0;
			for (size_t row = count; row > 0; row--) {
				if (m_ctrls.Index(row) == wxNOT_FOUND) continue;
				traverser.OnBook(Items(row - 1), 0, row);
				check_count++;
			}
			return check_count;
		}
	}
}

int FbBookListModel::GetBook()
{
	FbModelItem item = GetCurrent();
	FbBookListData * book = wxDynamicCast(&item, FbBookListData);
	return book ? book->GetCode() : 0;
}

void FbBookListModel::Delete()
{
	size_t count = m_items.Count();
	if (count == 0) return ;

	if (m_check.Count() > 0) {
		for (size_t i = 0; i < count; i++) {
			size_t index = count - i - 1;
			int book = m_items[index];
			if (m_check.Index(book) != wxNOT_FOUND) {
				if (0 <= index && index < count) m_items.RemoveAt(index);
				if (m_position > index) m_position--;
			}
		}
		m_check.Empty();
		return;
	}

	if (m_position == 0) return ;

	if (m_shift) {
		size_t min = m_shift < m_position ? m_shift : m_position;
		size_t max = m_shift > m_position ? m_shift : m_position;
		for (size_t i = 0; i <= max - min; i++) {
			size_t index = max - i - 1;
			if (0 <= index && index < count) m_items.RemoveAt(index);
			if (m_position > index) m_position--;
		};
		m_shift = 0;
	} else {
		size_t ctrls_count = m_ctrls.Count();
		if (ctrls_count) {
			for (size_t i = 0; i < ctrls_count; i++) {
				size_t index = m_ctrls[ctrls_count - i - 1] - 1;
				if (0 <= index && index < count) m_items.RemoveAt(index);
				if (m_position > index) m_position--;
			}
			m_ctrls.Empty();
		} else {
			m_items.RemoveAt(m_position - 1);
			if (m_position == count) m_position--;
		}
	}
}

void FbBookListModel::Modify(int book, bool add)
{
	wxWindow * owner = GetOwner();
	int index = m_items.Index(book);
	if (add) {
		if (index == wxNOT_FOUND) {
			m_items.Add(book);
			if (m_items.Count() == 1) {
				m_position = 1;
				wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGED, 0);
				event.SetEventObject(m_owner);
				m_owner->GetEventHandler()->ProcessEvent(event);
			}
		}
	} else {
		if (index != wxNOT_FOUND) {
			int pos = m_position;
			m_items.RemoveAt(index);
			if (m_items.Count() < m_position) m_position--;
			if (owner && pos <= index) {
				wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGED, 0);
				event.SetEventObject(m_owner);
				m_owner->GetEventHandler()->ProcessEvent(event);
			}
		}
	}
}
