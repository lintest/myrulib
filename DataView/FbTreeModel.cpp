#include "FbTreeModel.h"
#include "FbBookModel.h"

// -----------------------------------------------------------------------------
// class FbTreeModelData
// -----------------------------------------------------------------------------

#define BOOK_CASHE_SIZE 64

FbTreeModelData::FbTreeModelData(wxSQLite3ResultSet &result)
{
	m_rowid = result.GetInt(0);
	m_bookid = result.GetInt(1);
	m_title = result.GetAsString(2);
	m_filesize = result.GetInt(3);
}

FbTreeModelData::FbTreeModelData(const FbTreeModelData &data) :
	m_rowid(data.m_rowid),
	m_bookid(data.m_bookid),
	m_title(data.m_title),
	m_authors(data.m_authors),
	m_filesize(data.m_filesize)
{
}

wxString FbTreeModelData::GetAuthors(wxSQLite3Database &database)
{
	if (!m_authors.IsEmpty()) return m_authors;

	wxString sql = wxT("SELECT full_name FROM authors WHERE id IN (SELECT id_author FROM books WHERE id=?) ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, (int)m_bookid);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		if (!m_authors.IsEmpty()) m_authors += wxT(", ");
		m_authors += result.GetString(0).Trim(true);
	}
	return m_authors;
}

wxString FbTreeModelData::Format(const int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) + wxT(" ") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

wxString FbTreeModelData::GetValue(unsigned int col)
{
	switch (col) {
		case FbTreeModel::COL_ROWID:
			return wxString::Format("%d", m_rowid);
		case FbTreeModel::COL_BOOKID:
			return wxString::Format("%d", m_bookid);
		case FbTreeModel::COL_TITLE:
			return m_title;
		case FbTreeModel::COL_SIZE:
			return Format(m_filesize);
		default:
			return wxEmptyString;
	}
}

// -----------------------------------------------------------------------------
// class FbTreeModelCashe
// -----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbTreeModelArray);

FbTreeModelCashe::FbTreeModelCashe(const wxString &filename)
	: m_rowid(0)
{
    m_database.Open(filename);
}

unsigned int FbTreeModelCashe::RowCount()
{
    wxSQLite3ResultSet result = m_database.ExecuteQuery(wxT("SELECT COUNT(id) FROM books"));
    return result.NextRow() ? result.GetInt(0) : 0;
}

FbTreeModelData FbTreeModelCashe::FindRow(unsigned int rowid)
{
	if ( m_rowid <= rowid && rowid < m_rowid + GetCount() ) return operator[]( rowid - m_rowid );

	m_rowid = rowid <= BOOK_CASHE_SIZE ? 1 : rowid - BOOK_CASHE_SIZE;

	Empty();
	wxString sql = wxT("SELECT rowid, id, title, file_size FROM books WHERE rowid>=? ORDER BY 1 LIMIT ?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, (wxLongLong)m_rowid);
	stmt.Bind(2, BOOK_CASHE_SIZE * 2);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FbTreeModelData * found = NULL;
	unsigned int id = rowid;
	while (result.NextRow()) {
		FbTreeModelData * data = new FbTreeModelData(result);
		while ( id < data->Id() ) {
			Add(new FbTreeModelData(id++));
		}
		if (id == rowid) found = data;
		Add(data);
		id++;
	}

	if (found) return *found;

	return rowid;
}

wxString FbTreeModelCashe::GetValue(unsigned int row, unsigned int col)
{
	return FindRow(row + 1).GetValue(col);
}

bool FbTreeModelCashe::GetValue(wxVariant &variant, unsigned int row, unsigned int col)
{
	FbTreeModelData data = FindRow(row + 1);

    switch ( col ) {
		case FbTreeModel::COL_ROWID: {
			variant = wxString::Format("%d", row + 1);
		} break;
		case FbTreeModel::COL_TITLE: {
//			variant << FbTitleData( data.GetValue(col), m_checked.Index(row) != wxNOT_FOUND );
		} break;
		case FbTreeModel::COL_AUTHOR: {
			variant = data.GetAuthors(m_database);
		} break;
		default: {
			variant = data.GetValue(col);
		}
	}
	return true;
}

bool FbTreeModelCashe::SetValue(const wxVariant &variant, unsigned int row, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
//        if (data.m_checked) m_checked.Add(row);
//        else m_checked.Remove(row);
    }
    return true;
}

// -----------------------------------------------------------------------------
// class FbTreeModel
// -----------------------------------------------------------------------------

FbTreeModel::FbTreeModel(const wxString &filename)
{
    m_datalist = new FbTreeModelCashe(filename);
}

FbTreeModel::~FbTreeModel()
{
    wxDELETE(m_datalist);
}

void FbTreeModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
   	m_datalist->GetValue(variant, row, col);
}

bool FbTreeModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col )
{
   	return m_datalist->SetValue(variant, row, col);
}

bool FbTreeModel::GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const
{
	return false;

    switch ( col ) {
        case COL_ROWID:
            return false;

        case COL_TITLE:
            if ( !(row % 2) ) return false;
            attr.SetColour(*wxLIGHT_GREY);
            break;
    }

    return true;
}

