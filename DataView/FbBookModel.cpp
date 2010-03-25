#include "FbBookModel.h"

// -----------------------------------------------------------------------------
// class FbTitleData
// -----------------------------------------------------------------------------

IMPLEMENT_VARIANT_OBJECT(FbTitleData)

IMPLEMENT_DYNAMIC_CLASS(FbTitleData, wxObject)

// -----------------------------------------------------------------------------
// class FbBookModelData
// -----------------------------------------------------------------------------

#define BOOK_CASHE_SIZE 64

FbBookModelData::FbBookModelData(unsigned int id, wxSQLite3ResultSet &result)
{
	m_rowid    = id;
	m_bookid   = result.GetInt(0);
	m_title    = result.GetString(1);
	m_AuthIds  = result.GetString(2);
	m_filesize = result.GetInt(3);
}

FbBookModelData::FbBookModelData(const FbBookModelData &data) :
	m_rowid(data.m_rowid),
	m_bookid(data.m_bookid),
	m_title(data.m_title),
	m_authors(data.m_authors),
	m_AuthIds(data.m_AuthIds),
	m_filesize(data.m_filesize)
{
}

wxString FbBookModelData::GetAuthors(wxSQLite3Database &database)
{
	if (!m_authors.IsEmpty()) return m_authors;
	if (m_AuthIds.IsEmpty()) return m_authors;

	wxString sql = wxString::Format(wxT("SELECT FullName FROM Auth WHERE AuthId IN (%s) ORDER BY SearchName"), m_AuthIds.c_str());
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		if (!m_authors.IsEmpty()) m_authors += wxT(", ");
		m_authors += result.GetString(0).Trim(true);
	}
	return m_authors;
}

wxString FbBookModelData::Format(const int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) + wxT(" ") + wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

wxString FbBookModelData::GetValue(unsigned int col)
{
	switch (col) {
		case FbBookModel::COL_ROWID:
			return wxString::Format("%d", m_rowid);
		case FbBookModel::COL_BOOKID:
			return wxString::Format("%d", m_bookid);
		case FbBookModel::COL_TITLE:
			return m_title;
		case FbBookModel::COL_SIZE:
			return Format(m_filesize);
		default:
			return wxEmptyString;
	}
}

// -----------------------------------------------------------------------------
// class FbBookModelCashe
// -----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbBookModelArray);

FbBookModelCashe::FbBookModelCashe(const wxString &filename)
	: m_rowid(0)
{
    m_database.Open(filename);
}

unsigned int FbBookModelCashe::RowCount()
{
    m_database.ExecuteUpdate(wxT("CREATE TEMP TABLE TmpBook(BookId integer)"));
    return m_database.ExecuteUpdate(wxT("INSERT INTO TmpBook(BookId) SELECT BookId FROM Book ORDER BY Title"));
}

FbBookModelData FbBookModelCashe::FindRow(unsigned int rowid)
{
    for (size_t i=0; i<Count(); i++) {
        FbBookModelData data = Item(i);
        if (data.Id()==rowid) return data;
    }

    while (Count() > BOOK_CASHE_SIZE) RemoveAt(BOOK_CASHE_SIZE);

    int bookid = 0;
    {
        wxString sql = wxT("SELECT BookId FROM TmpBook WHERE RowId=?");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, (wxLongLong)rowid);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) bookid = result.GetInt(0);
    }

    if (bookid) {
        wxString sql = wxT("SELECT BookId, Title, AuthIds, FileSize FROM Book WHERE BookId=?");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, bookid);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) {
            FbBookModelData data(rowid, result);
            Insert(data, 0);
            return data;
        }
    }
	return rowid;
}

wxString FbBookModelCashe::GetValue(unsigned int row, unsigned int col)
{
	return FindRow(row + 1).GetValue(col);
}

bool FbBookModelCashe::GetValue(wxVariant &variant, unsigned int row, unsigned int col)
{
	FbBookModelData data = FindRow(row + 1);

    switch ( col ) {
		case FbBookModel::COL_ROWID: {
			variant = wxString::Format("%d", row + 1);
		} break;
		case FbBookModel::COL_TITLE: {
			variant << FbTitleData( data.GetValue(col), m_checked.Index(row) != wxNOT_FOUND );
		} break;
		case FbBookModel::COL_AUTHOR: {
			variant = data.GetAuthors(m_database);
		} break;
		default: {
			variant = data.GetValue(col);
		}
	}
	return true;
}

bool FbBookModelCashe::SetValue(const wxVariant &variant, unsigned int row, unsigned int col)
{
    if (col == FbBookModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        if (data.m_checked) m_checked.Add(row);
        else m_checked.Remove(row);
    }
    return true;
}

// -----------------------------------------------------------------------------
// class FbBookModel
// -----------------------------------------------------------------------------

FbBookModel::FbBookModel(const wxString &filename)
	: wxDataViewVirtualListModel(Init(filename))
{
}

FbBookModel::~FbBookModel()
{
    wxDELETE(m_datalist);
}

long FbBookModel::Init(const wxString &filename)
{
    m_datalist = new FbBookModelCashe(filename);
    return m_datalist->RowCount();
}

void FbBookModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
   	m_datalist->GetValue(variant, row, col);
}

bool FbBookModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col )
{
   	return m_datalist->SetValue(variant, row, col);
}

bool FbBookModel::GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const
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

