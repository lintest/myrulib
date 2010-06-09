#include "FbSeqnList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbSeqnListThread
//-----------------------------------------------------------------------------

void * FbSeqnListThread::Entry()
{
	try {
		FbCommonDatabase database;
		if (!m_string.IsEmpty() && IsFullText()) {
			DoFullText(database);
		} else {
			DoString(database);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbSeqnListThread::DoString(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, value, number FROM sequences");
	if (!m_string.IsEmpty()) sql << wxT(" WHERE SEARCH(search_name)");
	sql << GetOrder(wxT("value,number"), m_order);
	FbSearchFunction search(m_string);
	if (!m_string.IsEmpty()) database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
}

void FbSeqnListThread::DoFullText(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, value, number FROM fts_seqn LEFT JOIN sequences ON id=docid WHERE fts_seqn MATCH ?");
	sql << GetOrder(wxT("value,number"), m_order);
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
		if (m_tester.Closed()) return;
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddSeqn(new FbCacheData(code, result));
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

wxString FbSeqnListThread::GetOrder(const wxString &fields, int column)
{
	int i = 0;
	int number = column == 0 ? 1 : abs(column);
	wxString result = wxT(" ORDER BY ");
	wxString first;
	wxStringTokenizer tkz(fields, wxT(','));
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
	FbSeqnListModel * list = wxDynamicCast(&model, FbSeqnListModel);
	if (list == NULL) return wxEmptyString;

	FbCacheData * data = FbCollection::GetSeqn(m_code);
	if (data == NULL) return wxEmptyString;

	return data->GetValue(col);
}

//-----------------------------------------------------------------------------
//  FbSeqnListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnListModel, FbListModel)

FbSeqnListModel::FbSeqnListModel(const wxArrayInt &items)
	: m_data(NULL)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
}

FbSeqnListModel::~FbSeqnListModel(void)
{
	wxDELETE(m_data);
}

void FbSeqnListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

FbModelData * FbSeqnListModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_items.Count()) return NULL;
	int code = m_items[row - 1];
	wxDELETE(m_data);
	return m_data = new FbSeqnListData(code);
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
