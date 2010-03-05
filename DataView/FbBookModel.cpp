#include "FbBookModel.h"

// -----------------------------------------------------------------------------
// class FbBookModelData
// -----------------------------------------------------------------------------

FbBookModelData::FbBookModelData(const wxSQLite3ResultSet &res)
{
}

FbBookModelData::FbBookModelData(const FbBookModelData &data)
    : m_rowid(data.m_rowid), m_values(data.m_values)
{
}

wxString FbBookModelData::GetValue(unsigned int col)
{
	return col > m_values.Count() ? (wxString)wxEmptyString : m_values[col+1] ;
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

unsigned int FbBookModelCashe::GetCount()
{
    wxSQLite3ResultSet res = m_database.ExecuteQuery(wxT("SELECT COUNT(id) FROM books"));
    return res.NextRow() ? res.GetInt(0) : 0;
}

FbBookModelData FbBookModelCashe::FindRow(unsigned int row)
{

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
    return m_datalist->GetCount();
}

void FbBookModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
	if ( col == COL_ROWID ) {
		variant = wxString::Format( "virtual row %d", row + 1 );
	} else {
		FbBookModelData data = m_datalist->FindRow(row);
		if ( data.IsOk() ) variant = data.GetValue(col);
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

