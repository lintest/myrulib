#include "FbAuthListModel.h"

//-----------------------------------------------------------------------------
//  FbAuthListModelData
//-----------------------------------------------------------------------------

FbAuthListModelData::FbAuthListModelData(unsigned int id, wxSQLite3ResultSet &result) :
	m_rowid(id),
	m_auth(result.GetInt(0)),
	m_name(result.GetString(1)),
	m_count(result.GetInt(2))
{
}

wxString FbAuthListModelData::GetValue(size_t col)
{
	switch (col) {
		case 0: 
			return m_name;
		default: 
			return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbAuthListModelArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbAuthListModelArray) 

//-----------------------------------------------------------------------------
//  FbAuthListModel
//-----------------------------------------------------------------------------

FbAuthListModel::FbAuthListModel(const wxString &filename, const wxString &order, const wxString &letter)
{
	m_database.Open(filename);
    m_database.ExecuteUpdate(wxT("CREATE TEMP TABLE TmpAuth(AuthId integer)"));

	wxString sql = wxT("INSERT INTO TmpAuth(AuthId) SELECT AuthId FROM Auth");
	if (!letter.IsEmpty()) sql += wxT(" WHERE Letter=?");
	if (!order.IsEmpty()) sql += wxT(" ORDER BY ") + order;
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	if (!letter.IsEmpty()) stmt.Bind(1, letter);
	int count = stmt.ExecuteUpdate();
	if (count) SetRowCount((size_t)count);
}

FbAuthListModel::~FbAuthListModel(void)
{
}

FbAuthListModelData * FbAuthListModel::FindRow(size_t rowid)
{
	FbAuthListModelData * data = NULL;
    for (size_t i=0; i < m_data.Count(); i++) {
        if (m_data[i].GetId() == rowid) {
			FbAuthListModelData * data = m_data.Detach(i);
			m_data.Insert(data, 0);
            return data;
        }
    }

	int bookid = m_data.Count();
    if (m_data.Count() > TREELIST_CASHE_SIZE) 
		m_data.RemoveAt(TREELIST_CASHE_SIZE, m_data.Count() - TREELIST_CASHE_SIZE);

    bookid = m_data.Count();
    {
        wxString sql = wxT("SELECT AuthId FROM TmpAuth WHERE RowId=?");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, (wxLongLong)rowid);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) bookid = result.GetInt(0);
    }

    if (bookid) {
        wxString sql = wxT("SELECT AuthId, FullName, BookCount FROM Auth WHERE AuthId=?");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, bookid);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) {
			FbAuthListModelData * data = new FbAuthListModelData(rowid, result);
			m_data.Insert(data, 0);
			return data;
		}
    }

	return NULL;
}

FbAuthListModelData * FbAuthListModel::FindRow(const FbTreeItemId &id)
{
	FbTreeItemKeyList * key = FbTreeItemKeyList::Key(id);
	return key ? FindRow(key->GetId()) : NULL;
}

wxString FbAuthListModel::GetValue(const FbTreeItemId &id, size_t col)
{
	FbAuthListModelData * data = FindRow(id);
	if (data) return data->GetValue(col);
	return wxEmptyString;
}

FbTreeItemId FbAuthListModel::GetCurrent()
{
	FbAuthListModelData * data = FindRow(m_current);
	if (data) return FbTreeItemKeyAuth(data->GetAuth());
	return FbTreeItemId();
}
