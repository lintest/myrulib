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
	FbCommonDatabase database;
	if (m_info.m_author) {
		DoAuthor(database);
	} else if (m_info.m_string.IsEmpty()) {
		DoLetter(database);
	} else if (m_info.IsFullText()) {
		DoFullText(database);
	} else {
		DoString(database);
	}
	return NULL;
}

void FbAuthListThread::DoAuthor(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_info.m_author);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbAuthListThread::DoLetter(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors");
	if (m_info.m_letter) sql << wxT(' ') << wxT("WHERE letter=?");
	sql << GetOrder(wxT("search_name,number"), m_order);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_info.m_letter) stmt.Bind(1, (wxString)m_info.m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbAuthListThread::DoString(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE SEARCH(search_name)");
	sql << GetOrder(wxT("search_name,number"), m_order);
	FbSearchFunction search(m_info.m_string);
	database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	MakeModel(result);
}

void FbAuthListThread::DoFullText(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT docid, full_name, number FROM fts_auth INNER JOIN authors ON id=docid WHERE fts_auth MATCH ?");
	sql << GetOrder(wxT("search_name,number"), m_order);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_info.m_string));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	MakeModel(result);
}

void FbAuthListThread::MakeModel(wxSQLite3ResultSet &result)
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
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

wxString FbAuthListThread::GetOrder(const wxString &fields, int column)
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
//  FbAuthListData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbAuthListData, FbModelData)

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	return FbCollection::GetAuth(m_code, col);
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

FbViewItem FbAuthListModel::GetView()
{
	if (m_position && m_position <= m_items.Count()) {
		return FbViewItem(FbViewItem::Book, m_items[m_position - 1]);
	} else return FbViewItem::None;
};
