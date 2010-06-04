#include "FbAuthList.h"

#define AUTH_CACHE_SIZE 100

//-----------------------------------------------------------------------------
//  FbAuthListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListData, FbModelData)

FbAuthListData::FbAuthListData(int code, wxSQLite3Database &database)
	: m_code(code)
{
	try {
		wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_code);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) {
			m_name = result.GetString(1);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

FbAuthListData::FbAuthListData(int code, const wxString &name)
	: m_code(code), m_name(name)
{
}

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return m_name;
		case 1:
			return Format(m_code);
		default:
			return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbAuthListArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbAuthListArray);

//-----------------------------------------------------------------------------
//  FbAuthListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListModel, FbListModel)

FbAuthListModel::FbAuthListModel(int order, wxChar letter)
{
	try {
		wxString condition;
		if (letter) condition = wxT("Letter=?");
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
			if (count >= AUTH_CACHE_SIZE) continue;
			wxString name = result.GetString(1);
			m_cache.Add(new FbAuthListData(code, name));
			count++;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}
/*
FbAuthListModel::FbAuthListModel(int order, const wxString &mask)
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
	size_t count = m_cache.Count();
	for (size_t i = 0; i < count; i++) {
		FbAuthListData & data = m_cache[i];
		if (data.GetCode() == code) return &data;
	}
	FbAuthListData * data = new FbAuthListData(code, m_database);
	m_cache.Insert(data, 0);
	if (count > AUTH_CACHE_SIZE) m_cache.RemoveAt(AUTH_CACHE_SIZE, count - AUTH_CACHE_SIZE);
	return data;
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
