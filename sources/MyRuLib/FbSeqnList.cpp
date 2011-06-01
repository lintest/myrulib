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
	wxString sql = wxT("SELECT id_seq, COUNT(DISTINCT id_book) FROM bookseq GROUP BY id_seq");

	FbCommonDatabase database;
	database.JoinThread(this);

	if (abs(m_order) > 1) {
		if (m_counter.IsEmpty()) {
			CreateCounter(database, sql);
		} else {
			AttachCounter(database, m_counter);
		}
	}

	if (!m_string.IsEmpty() && IsFullText(database)) {
		DoFullText(database);
	} else {
		DoString(database);
	}

	if (m_counter.IsEmpty()) {
		CreateCounter(database, sql);
	}

	return NULL;
}

void FbSeqnListThread::DoString(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, value FROM sequences");
	sql << GetJoin();
	if (!m_string.IsEmpty()) sql << wxT("WHERE SEARCH(value)");
	sql << GetOrder();
	FbSearchFunction search(m_string);
	if (!m_string.IsEmpty()) database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
}

void FbSeqnListThread::DoFullText(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, value FROM fts_seqn INNER JOIN sequences ON id=docid");
	sql << GetJoin();
	sql << wxT("WHERE fts_seqn MATCH ?");
	sql << GetOrder();
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_string));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbSeqnListThread::MakeModel(wxSQLite3ResultSet &result)
{
	if (!result.IsOk()) return;
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return;
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddSeqn(code, result.GetString(1));
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

wxString FbSeqnListThread::GetJoin()
{
	return abs(m_order) > 1 ? wxT(" LEFT JOIN cnt.numb ON id=numb.key ") : wxString(wxT(' '));
}

wxString FbSeqnListThread::GetOrder()
{
	return FbFrameThread::GetOrder(m_order, wxT("value") + fbCOLLATE_CYR);
}

//-----------------------------------------------------------------------------
//  FbSeqnListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbSeqnListData, FbModelData)

wxString FbSeqnListData::GetValue(FbModel & model, size_t col) const
{
	if (col == 1) {
		FbSeqnListModel * master = wxDynamicCast(&model, FbSeqnListModel);
		if (master) {
			int count = master->GetCount(m_code);
			if (count != wxNOT_FOUND) return FbCollection::Format(count);
		}
		return wxEmptyString;
	} else {
		return FbCollection::GetSeqn(m_code, col);
	}
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
	if (m_counter.count(code)) return m_counter[code];
	int count = FbFrameThread::GetCount(m_database, code);
	if (count == wxNOT_FOUND) return wxNOT_FOUND;
	m_counter[code] = count;
	return count;
}

