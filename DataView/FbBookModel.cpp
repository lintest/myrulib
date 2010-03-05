#include "FbBookModel.h"

FbBookModel::FbBookModel(const wxString &filename)
{
    m_database.Open(filename);
    m_count = m_database.ExecuteScalar(wxT("SELECT COUNT(*) FROM books"));
//	wxSQLite3ResultSet result = database.ExecuteQuery();
}
