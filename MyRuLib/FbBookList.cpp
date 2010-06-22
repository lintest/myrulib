#include "FbBookList.h"
#include "FbBookData.h"
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
	size_t count = m_items.Count();
	if (count == 0) return 0;

	size_t check_count = m_check.Count();
	for (size_t i = 0; i < check_count; i++) {
		items.Add(m_check[i]);
	}
	if (check_count) return check_count;

	if (m_position == 0) return 0;
	if (m_shift) {
		size_t min = m_shift < m_position ? m_shift : m_position;
		size_t max = m_shift > m_position ? m_shift : m_position;
		for (size_t i = min; i <= max; i++) items.Add(m_items[i - 1]);
	} else {
		size_t ctrls_count = m_ctrls.Count();
		if (ctrls_count) {
			for (size_t i = 0; i < ctrls_count; i++) {
				size_t index = m_ctrls[i] - 1;
				if (0 <= index && index < count) items.Add(m_items[index]);
			}
		} else {
			items.Add(m_items[m_position - 1]);
		}
	}
	return items.Count();
}

