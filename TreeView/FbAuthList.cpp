#include "FbAuthList.h"
#include "FbCollection.h"
#include "FbBookEvent.h"
#include <wx/tokenzr.h>

#define AUTH_CACHE_SIZE 64

//-----------------------------------------------------------------------------
//  FbAuthListThread
//-----------------------------------------------------------------------------

void * FbAuthListThread::Entry()
{
	try {
		FbCommonDatabase database;
		if (m_info.m_string.IsEmpty()) 
			DoLetter(database);
		else 
			DoString(database);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbAuthListThread::DoLetter(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors");
	if (m_info.m_letter) sql << wxT(' ') << wxT("WHERE letter=?");
	sql << GetOrder(wxT("search_name,number"), m_order);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_info.m_letter) stmt.Bind(1, (wxString)m_info.m_letter);
	MakeModel(stmt.ExecuteQuery());
}

void FbAuthListThread::DoString(wxSQLite3Database &database)
{
	if ( FbSearchFunction::IsFullText(m_info.m_string) ) {
		wxString sql = wxT("SELECT docid, full_name, number FROM fts_auth LEFT JOIN authors ON id=docid WHERE fts_auth MATCH ?");
		sql << GetOrder(wxT("search_name,number"), m_order);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, FbSearchFunction::AddAsterisk(m_info.m_string));
		MakeModel(stmt.ExecuteQuery());
	} else {
		wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE SEARCH(search_name)");
		sql << GetOrder(wxT("search_name,number"), m_order);
		FbSearchFunction search(m_info.m_string);
		database.CreateFunction(wxT("SEARCH"), 1, search);
		MakeModel(database.ExecuteQuery(sql));
	}
}

void FbAuthListThread::MakeModel(wxSQLite3ResultSet &result)
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		int code = result.GetInt(0);
		if (id == ID_MODEL_CREATE) FbCollection::AddAuth(new FbCacheData(code, result));
		items.Add(code);
		count++;
		if (count == AUTH_CACHE_SIZE) {
			FbArrayEvent(id, items).Post(m_frame);
			id = ID_MODEL_APPEND;
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(id, items).Post(m_frame);
}

wxString FbAuthListThread::GetOrder(const wxString &fields, int column)
{
	int i = 0;
	int number = column == 0 ? 1 : abs(column);
	wxString result = wxT(" ORDER BY ");
	wxString first;
	wxStringTokenizer tkz(fields, wxT(","));
	while (tkz.HasMoreTokens()) {
		i++;
		wxString token = tkz.GetNextToken();
		if (column < 0) token << wxT(' ') << wxT("desc");
		if (i == number) result << token;
		if (i == 1) first = token;
	}
	if (number != 1) result << wxT(',') << first;
	return result;
}

//-----------------------------------------------------------------------------
//  FbAuthListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListData, FbModelData)

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	FbAuthListModel * list = wxDynamicCast(&model, FbAuthListModel);
	if (list == NULL) return wxEmptyString;

	FbCacheData * data = FbCollection::GetAuth(m_code);
	if (data == NULL) return wxEmptyString;

	return data->GetValue(col);
}

//-----------------------------------------------------------------------------
//  FbAuthListModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListModel, FbListModel)

FbAuthListModel::FbAuthListModel(const wxArrayInt &items)
	: m_data(NULL)
{
	m_position = items.Count() == 0 ? 0 : 1;
	Append(items);
}

FbAuthListModel::~FbAuthListModel(void)
{
	wxDELETE(m_data);
}

void FbAuthListModel::Append(const wxArrayInt &items)
{
	WX_APPEND_ARRAY(m_items, items);
}

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
