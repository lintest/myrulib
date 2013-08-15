#include "FbAuthList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbAuthListThread
//-----------------------------------------------------------------------------

void * FbAuthListThread::Entry()
{
	bool calculate = m_counter.IsEmpty();
	FbFrameDatabase database(this, m_counter);

	if (calculate && abs(m_order) > 1) {
		CreateCounter(database, m_sql);
		if (IsClosed()) return NULL;
		calculate = false;
	}

	if (m_info.m_string.IsEmpty()) {
		DoLetter(database);
	} else if (m_info.IsFullText()) {
		DoFullText(database);
	} else {
		DoString(database);
	}

	if (calculate) {
		CreateCounter(database, m_sql);
	}

	return NULL;
}

void FbAuthListThread::DoLetter(FbSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors");
	sql << GetJoin();
	if (m_info.m_letter) sql << wxT("WHERE letter=?");
	sql << GetOrder();
	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_info.m_letter) stmt.Bind(1, (wxString)m_info.m_letter);
	FbSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbAuthListThread::DoString(FbSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors");
	sql << GetJoin();
	sql << wxT("WHERE SEARCH(full_name)");
	sql << GetOrder();
	FbSearchFunction search(m_info.m_string);
	database.CreateFunction(wxT("SEARCH"), 1, search);
	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
}

void FbAuthListThread::DoFullText(FbSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, full_name FROM fts_auth INNER JOIN authors ON id=docid");
	sql << GetJoin();
	sql << wxT("WHERE fts_auth MATCH ?");
	sql << GetOrder();
	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.FTS(1, m_info.m_string);
	FbSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbAuthListThread::MakeModel(FbSQLite3ResultSet &result)
{
	if (!result.IsOk()) return;
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return;
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddAuth(code, result.GetString(1).Trim(true));
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

wxString FbAuthListThread::GetJoin()
{
	return abs(m_order) > 1 ? wxT(" LEFT JOIN numb ON id=numb.key ") : wxString(wxT(' '));
}

wxString FbAuthListThread::GetOrder()
{
	return FbFrameThread::GetOrder(m_order, wxT("full_name") + fbCOLLATE_CYR);
}

//-----------------------------------------------------------------------------
//  FbAuthListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListData, FbModelData)

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	if (col == 1) {
		FbAuthListModel * master = wxDynamicCast(&model, FbAuthListModel);
		if (master) {
			int count = master->GetCount(m_code);
			if (count != wxNOT_FOUND) return FbCollection::Format(count);
		}
		return wxEmptyString;
	} else {
		return FbCollection::GetAuth(m_code, col);
	}
}

//-----------------------------------------------------------------------------
//  FbAuthListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListModel, FbListModel)

FbAuthListModel::FbAuthListModel(const wxArrayInt &items, int code)
{
	m_position = items.Count() ? 1 : 0;
	Append(items);
	if (code) {
		int i = items.Index(code);
		if (i != wxNOT_FOUND) m_position = (size_t)i + 1;
	}
}

FbAuthListModel::~FbAuthListModel(void)
{
}

void FbAuthListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

void FbAuthListModel::Append(FbModelData * data)
{
	FbAuthListData * item = wxDynamicCast(data, FbAuthListData);
	if (item && m_position > 0) m_items.Insert(item->GetCode(), m_position - 1);
	wxDELETE(data);
}

void FbAuthListModel::Replace(FbModelData * data)
{
	FbAuthListData * item = wxDynamicCast(data, FbAuthListData);
	if (item && m_position > 0) m_items[m_position - 1] = item->GetCode();
	wxDELETE(data);
}

void FbAuthListModel::Delete()
{
	size_t count = m_items.Count();
	if (m_position && m_position <= count) {
		m_items.RemoveAt(m_position - 1);
		if (m_position >= count) m_position = count - 1;
	}
}

FbModelItem FbAuthListModel::DoGetData(size_t row, int &level)
{
	level = 0;
	if (row == 0 || row > m_items.Count()) return *this;
	int code = m_items[row - 1];
	FbAuthListData data(code);
	return FbModelItem(*this, &data);
}

void FbAuthListModel::Delete(int code)
{
	int index = m_items.Index(code);
	if (index == wxNOT_FOUND) return;
	if (index < (int)m_position) m_position--;
	m_items.RemoveAt(index);
}

void FbAuthListModel::SetCount(int code, int count)
{
	m_counter[code] = count;
}

int FbAuthListModel::GetCount(int code)
{
	if (m_counter.count(code)) return m_counter[code];
	int count = FbFrameThread::GetCount(m_database, code);
	if (count == wxNOT_FOUND) return wxNOT_FOUND;
	m_counter[code] = count;
	return count;
}

void FbAuthListModel::SetCounter(const wxString & filename)
{
	if (!filename.IsEmpty()) m_database.Open(filename);
	m_counter.clear();
}
