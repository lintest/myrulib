#include "FbBookModel.h"

FbBookModel::FbBookModel(const wxString &filename)
    : wxDataViewVirtualListModel(InitDatabase(filename))
{
}

FbBookModel::~FbBookModel()
{
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
        case Col_EditableText:
            variant = wxString::Format( "virtual row %d", row + 1 );
            break;

        case Col_IconText: {
            wxString sql = wxT("SELECT rowid, title FROM books WHERE rowid=?");
            wxSQLite3Statement stmt = m_database->PrepareStatement(sql);
            stmt.Bind(1, (wxLongLong)row + 1);
            wxSQLite3ResultSet res = stmt.ExecuteQuery();
            if (res.NextRow()) {
                variant = res.GetString(1);
           }
        } break;

        case Col_TextWithAttr:
            {
                static const char *labels[5] =
                {
                    "blue", "green", "red", "bold cyan", "default",
                };

                variant = labels[row % 5];
            }
            break;

        case Col_Custom:
            variant = wxString::Format("%d", row % 100);
            break;

        case Col_Max:
            wxFAIL_MSG( "invalid column" );
    }
}

bool FbBookModel::GetAttrByRow( unsigned int row, unsigned int col,
                                wxDataViewItemAttr &attr ) const
{
    switch ( col )
    {
        case Col_EditableText:
            return false;

        case Col_IconText:
            if ( !(row % 2) )
                return false;
            attr.SetColour(*wxLIGHT_GREY);
            break;

        case Col_TextWithAttr:
        case Col_Custom:
            // do what the labels defined in GetValueByRow() hint at
            switch ( row % 5 )
            {
                case 0:
                    attr.SetColour(*wxBLUE);
                    break;

                case 1:
                    attr.SetColour(*wxGREEN);
                    break;

                case 2:
                    attr.SetColour(*wxRED);
                    break;

                case 3:
                    attr.SetColour(*wxCYAN);
                    attr.SetBold(true);
                    break;

                case 4:
                    return false;
            }
            break;

        case Col_Max:
            wxFAIL_MSG( "invalid column" );
    }

    return true;
}

