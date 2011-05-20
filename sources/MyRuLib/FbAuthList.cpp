#include "FbAuthList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include <wx/tokenzr.h>

//-----------------------------------------------------------------------------
//  FbAuthListThread
//-----------------------------------------------------------------------------

static void AttachCounter(wxSQLite3Database &database, const wxString &filename)
{
	wxSQLite3Statement stmt = database.PrepareStatement(wxT("ATTACH ? AS cnt"));
	stmt.Bind(1, filename);
	stmt.ExecuteUpdate();
}

void * FbAuthListThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);

	if (abs(m_order) > 1) {
		if (m_counter.IsEmpty()) {
			CreateCounter(database);
		} else {
			AttachCounter(database, m_counter);
		}
	}

	if (m_info.m_string.IsEmpty()) {
		DoLetter(database);
	} else if (m_info.IsFullText()) {
		DoFullText(database);
	} else {
		DoString(database);
	}

	return NULL;
}

void FbAuthListThread::DoLetter(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, 0 FROM authors");
	sql << GetJoin() << wxT("WHERE letter=?") << GetOrder();
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_info.m_letter) stmt.Bind(1, (wxString)m_info.m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
	CreateCounter(database);
}

void FbAuthListThread::DoString(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, 0 FROM authors");
	sql << GetJoin() << wxT("WHERE SEARCH(search_name)") << GetOrder();
	FbSearchFunction search(m_info.m_string);
	database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
	CreateCounter(database);
}

void FbAuthListThread::DoFullText(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, full_name, 0 FROM fts_auth INNER JOIN authors ON id=docid");
	sql << GetJoin() << wxT("WHERE fts_auth MATCH ?") << GetOrder();
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_info.m_string));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
	CreateCounter(database);
}

void FbAuthListThread::MakeModel(wxSQLite3ResultSet &result)
{
	if (!result.IsOk()) return;
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (IsClosed()) return;
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddAuth(new FbCacheData(result));
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
	return abs(m_order) > 1 ? wxT(" LEFT JOIN cnt.a ON aid=id ") : wxString(wxT(' '));
}

wxString FbAuthListThread::GetOrder()
{
	wxString fields = wxT("full_name COLLATE CYR,a.num");
	int number = m_order == 0 ? 1 : abs(m_order);
	wxString result = wxT(" ORDER BY ");
	wxString first;
	wxStringTokenizer tkz(fields, wxT(','));
	int i = 0;
	while (tkz.HasMoreTokens()) {
		i++;
		wxString token = tkz.GetNextToken();
		if (m_order < 0) token << wxT(" desc");
		if (i == number) result << token;
		if (i == 1) first = token;
	}
	if (number != 1) result << wxT(',') << first;
	return result;
}

void FbAuthListThread::CreateCounter(wxSQLite3Database &database)
{
	if (!m_counter.IsEmpty()) return;

	m_counter = wxFileName::CreateTempFileName(wxT("fb"));
	AttachCounter(database, m_counter);

	wxString sql = wxT("CREATE TABLE cnt.a(aid INTEGER PRIMARY KEY, num INTEGER)");
	database.ExecuteUpdate(sql);

	sql = wxT("INSERT INTO cnt.a(aid, num) SELECT id_author, COUNT(id) FROM books GROUP BY id_author");
	database.ExecuteUpdate(sql);

	if (IsClosed()) {
		wxRemoveFile(m_counter);
		m_counter = wxEmptyString;
	} else {
		FbCommandEvent(fbEVT_BOOK_ACTION, ID_MODEL_NUMBER, m_counter).Post(m_frame);
	}
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

FbAuthListModel::FbAuthListModel(const wxArrayInt &items)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
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
	if (!m_database.IsOpen()) return wxNOT_FOUND;

	wxString sql = wxT("SELECT num FROM a WHERE aid="); sql << code;
	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	int count = result.NextRow() ? result.GetInt(0) : 0;
	m_counter[code] = count;
	return count;
}

