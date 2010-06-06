#include "FbAuthList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"

#define AUTH_CACHE_SIZE 64

//-----------------------------------------------------------------------------
//  FbAuthListThread
//-----------------------------------------------------------------------------

void * FbAuthListThread::Entry()
{
	try {
		FbCommonDatabase database;
		Load(database);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbAuthListThread::Load(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors");
	if (m_letter) sql << wxT(' ') << wxT("WHERE letter=?");
	sql << wxT(' ') << wxT("ORDER BY") << wxT(' ') << GetOrder(m_order);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_letter) stmt.Bind(1, (wxString)m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	size_t count = 0;
	wxArrayInt items;
	FbAuthListModel * model = new FbAuthListModel(items);
	while (result.NextRow()) {
		int code = result.GetInt(0);
		model->GetCollection().AddAuth(new FbCacheData(code, result));
		items.Add(code);
		count++;
		if (count == AUTH_CACHE_SIZE) break;
	}
	model->Append(items);
	FbModelEvent(ID_MASTER_MODEL, model).Post(m_frame);
	items.Empty();
	count = 0;

	while (result.NextRow()) {
		int code = result.GetInt(0);
		items.Add(code);
		count++;
		if (count == AUTH_CACHE_SIZE) {
			FbArrayEvent(ID_MASTER_MODEL, items).Post(m_frame);
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(ID_MASTER_MODEL, items).Post(m_frame);
}

wxString FbAuthListThread::GetOrder(int column)
{
	switch (column) {
		case -2: return wxT("number desc, search_name desc");
		case -1: return wxT("search_name desc");
		case  2: return wxT("number, search_name");
		default: return wxT("search_name ");
	}
}

//-----------------------------------------------------------------------------
//  FbAuthListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListData, FbModelData)

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	FbAuthListModel * list = wxDynamicCast(&model, FbAuthListModel);
	if (list == NULL) return wxEmptyString;

	FbCacheData * data = list->GetCollection().GetAuth(m_code);
	if (data == NULL) return wxEmptyString;

	return data->GetValue(col);
}

//-----------------------------------------------------------------------------
//  FbAuthListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListModel, FbListModel)

FbAuthListModel::FbAuthListModel(wxArrayInt &items)
	: m_data(NULL)
{
	m_position = items.Count();
	Append(items);
}

FbAuthListModel::FbAuthListModel(int order, wxChar letter)
	: m_data(NULL)
{
	try {
		wxString sql = wxT("SELECT id, full_name, number FROM authors");
		if (letter) sql << wxT(' ') << wxT("WHERE letter=?");
		sql << wxT(' ') << wxT("ORDER BY") << wxT(' ') << GetOrder(order);
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		if (letter) stmt.Bind(1, (wxString)letter);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		size_t count = 0;
		while (result.NextRow()) {
			int code = result.GetInt(0);
			m_items.Add(code);
			if (count == 1) m_position = 1;
			if (count >= AUTH_CACHE_SIZE) continue;
			m_collection.AddAuth(new FbCacheData(code, result));
			count++;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}
/*
FbAuthListModel::FbAuthListModel(int order, const wxString &mask)
	: m_data(NULL)
{
	try {
		bool bFullText = FbSearchFunction::IsFullText(mask);

		if ( bFullText ) {
			wxString sql = wxT("SELECT docid FROM fts_auth WHERE fts_auth MATCH ? ORDER BY content");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, FbSearchFunction::AddAsterisk(mask));
			count = stmt.ExecuteUpdate();
		} else {
			wxString sql = GetSQL(GetOrder(order), wxT("SEARCH(search_name)"));
			FbSearchFunction search(mask);
			m_database.CreateFunction(wxT("SEARCH"), 1, search);
			count = m_database.ExecuteUpdate(sql);
		}
		if (count) SetRowCount((size_t)count);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		SetRowCount(0);
	}
}
*/
FbAuthListModel::~FbAuthListModel(void)
{
	wxDELETE(m_data);
}

void FbAuthListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

wxString FbAuthListModel::GetOrder(int column)
{
	switch (column) {
		case -2: return wxT("number desc, search_name desc");
		case -1: return wxT("search_name desc");
		case  2: return wxT("number, search_name");
		default: return wxT("search_name ");
	}
}
/*
wxString FbAuthListModel::GetSQL(const wxString & order, const wxString & condition)
{
	wxString sql = wxT("SELECT id, full_name FROM authors");
	if (!condition.IsEmpty()) sql += wxT(" WHERE ") + condition;
	if (!order.IsEmpty()) sql += wxT(" ORDER BY ") + order;
	return 	sql;
}
*/
FbModelData * FbAuthListModel::GetData(size_t row)
{
	if (row == 0 || row > m_items.Count()) return NULL;
	int code = m_items[row - 1];
	wxDELETE(m_data);
	return m_data = new FbAuthListData(code);
}

void FbAuthListModel::Delete()
{
	size_t count = m_items.Count();
	if (m_position && m_position <= count) {
		m_items.RemoveAt(m_position - 1);
		if (m_position >= count) m_position = count - 1;
		if (m_owner) m_owner->Refresh();
	}
}
