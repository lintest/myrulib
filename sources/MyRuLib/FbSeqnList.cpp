#include "FbSeqnList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbSeqnListThread
//-----------------------------------------------------------------------------

bool FbSeqnListThread::IsFullText(wxSQLite3Database &database) const
{
	return FbSearchFunction::IsFullText(m_string) && database.TableExists(wxT("fts_seqn"));
}

void * FbSeqnListThread::Entry()
{
	FbCommonDatabase database;
	if (!m_string.IsEmpty() && IsFullText(database)) {
		DoFullText(database);
	} else {
		DoString(database);
	}
	return NULL;
}

void FbSeqnListThread::DoString(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, value, number FROM sequences");
	if (!m_string.IsEmpty()) sql << wxT(" WHERE SEARCH(value)");
	sql << GetOrder(m_order);
	FbSearchFunction search(m_string);
	if (!m_string.IsEmpty()) database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
}

void FbSeqnListThread::DoFullText(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, value, number FROM fts_seqn INNER JOIN sequences ON id=docid WHERE fts_seqn MATCH ?");
	sql << GetOrder(m_order);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_string));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbSeqnListThread::MakeModel(wxSQLite3ResultSet &result)
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return;
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddSeqn(new FbCacheData(result));
		items.Add(code);
		count++;
		if (count == length) {
			length = fbLIST_ARRAY_SIZE;
			FbArrayEvent(id, items).Post(m_frame);
			id = ID_MODEL_APPEND;
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(id, items).Post(m_frame);
}

wxString FbSeqnListThread::GetOrder(int column)
{
	wxString fields = wxT("value COLLATE CYR,number");
	int number = column == 0 ? 1 : abs(column);
	wxString result = wxT(" ORDER BY ");
	wxString first;
	wxStringTokenizer tkz(fields, wxT(','));
	int i = 0;
	while (tkz.HasMoreTokens()) {
		i++;
		wxString token = tkz.GetNextToken();
		if (column < 0) token << wxT(" desc");
		if (i == number) result << token;
		if (i == 1) first = token;
	}
	if (number != 1) result << wxT(',') << first;
	return result;
}

//-----------------------------------------------------------------------------
//  FbSeqnListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnListData, FbModelData)

wxString FbSeqnListData::GetValue(FbModel & model, size_t col) const
{
	return FbCollection::GetSeqn(m_code, col);
}

//-----------------------------------------------------------------------------
//  FbSeqnListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnListModel, FbListModel)

FbSeqnListModel::FbSeqnListModel(const wxArrayInt &items)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
}

FbSeqnListModel::~FbSeqnListModel(void)
{
}

void FbSeqnListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

FbModelItem FbSeqnListModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_items.Count()) return *this;
	int code = m_items[row - 1];
	FbSeqnListData data(code);
	return FbModelItem(*this, &data);
}

void FbSeqnListModel::Append(FbModelData * data)
{
	FbSeqnListData * item = wxDynamicCast(data, FbSeqnListData);
	if (item && m_position > 0) m_items.Insert(item->GetCode(), m_position - 1);
	wxDELETE(data);
}

void FbSeqnListModel::Replace(FbModelData * data)
{
	FbSeqnListData * item = wxDynamicCast(data, FbSeqnListData);
	if (item && m_position > 0) m_items[m_position - 1] = item->GetCode();
	wxDELETE(data);
}

void FbSeqnListModel::Delete()
{
	size_t count = m_items.Count();
	if (m_position && m_position <= count) {
		m_items.RemoveAt(m_position - 1);
		if (m_position >= count) m_position = count - 1;
	}
}

void FbSeqnListModel::Delete(int code)
{
	int index = m_items.Index(code);
	if (index == wxNOT_FOUND) return;
	if (index < (int)m_position) m_position--;
	m_items.RemoveAt(index);
}

void FbSeqnListModel::SetCount(int code, int count)
{
	m_counter[code] = count;
}

int FbSeqnListModel::GetCount(int code)
{
	return m_counter.count(code) ? m_counter[code] : wxNOT_FOUND;
}

