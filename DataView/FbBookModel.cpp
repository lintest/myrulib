#include "FbBookModel.h"

// -----------------------------------------------------------------------------
// class FbBookModelArray
// -----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbBookModelArray);

FbBookModelData::FbBookModelData(const wxSQLite3ResultSet &res)
{
}

FbBookModelData::FbBookModelData(const FbBookModelData &data)
    : m_rowid(data.m_rowid), m_values(data.m_values)
{
}

// -----------------------------------------------------------------------------
// class FbBookModel
// -----------------------------------------------------------------------------

FbBookModel::FbBookModel(const wxString &filename) :
    wxDataViewVirtualListModel(InitDatabase(filename)),
    m_datalist(new FbBookModelArray)
{
}

FbBookModel::~FbBookModel()
{
    wxDELETE(m_datalist);
    wxDELETE(m_database);
}

long FbBookModel::InitDatabase(const wxString &filename)
{
    m_database = new wxSQLite3Database;
    m_database->Open(filename);
    wxSQLite3ResultSet res = m_database->ExecuteQuery(wxT("SELECT COUNT(id) FROM books"));
    return res.NextRow() ? res.GetInt(0) : 0;
}

void FbBookModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
    switch ( col )
    {
        case COL_ROWID: {
            variant = wxString::Format( "virtual row %d", row + 1 );
        } break;

        case COL_TITLE: {
            wxString sql = wxT("SELECT rowid, title FROM books WHERE rowid=?");
            wxSQLite3Statement stmt = m_database->PrepareStatement(sql);
            stmt.Bind(1, (wxLongLong)row + 1);
            wxSQLite3ResultSet res = stmt.ExecuteQuery();
            if (res.NextRow()) {
                variant = res.GetString(1);
           }
        } break;
    }
}

bool FbBookModel::GetAttrByRow( unsigned int row, unsigned int col, wxDataViewItemAttr &attr ) const
{
    switch ( col )
    {
        case COL_ROWID:
            return false;

        case COL_TITLE:
            if ( !(row % 2) ) return false;
            attr.SetColour(*wxLIGHT_GREY);
            break;
    }

    return true;
}

