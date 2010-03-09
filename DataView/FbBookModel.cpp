#include "FbBookModel.h"

// -----------------------------------------------------------------------------
// class FbTitleData
// -----------------------------------------------------------------------------

IMPLEMENT_VARIANT_OBJECT(FbTitleData)

IMPLEMENT_DYNAMIC_CLASS(FbTitleData, wxObject)

// -----------------------------------------------------------------------------
// class FbTitleRenderer
// -----------------------------------------------------------------------------

bool FbTitleRenderer::Render( wxRect rect, wxDC *dc, int state )
{
	dc->SetBrush( *wxLIGHT_GREY_BRUSH );
	dc->SetPen( *wxTRANSPARENT_PEN );

	rect.Deflate(2);
	dc->DrawRoundedRectangle( rect, 5 );

	int x = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth();

	wxRect checkbox = rect;
	checkbox.SetWidth(x);

	long flag = m_checked ? wxCONTROL_CHECKED : 0;
    wxRendererNative::Get().DrawCheckBox(GetOwner()->GetOwner(), *dc, checkbox, flag);

	RenderText(m_title, x + 2, rect, dc, state);
	return true;
}

bool FbTitleRenderer::LeftClick( wxPoint cursor, wxRect cell, wxDataViewModel *model, const wxDataViewItem &item, unsigned int col )
{
	int x = wxRendererNative::Get().GetCheckBoxSize(NULL).GetWidth();
	if (cursor.x - cell.GetX() > x + 4) return false;
    wxVariant variant;
    variant << FbTitleData( m_title, not m_checked);
    model->ChangeValue(variant, item, col);
    return true;
}

bool FbTitleRenderer::Activate( wxRect WXUNUSED(cell), wxDataViewModel *model, const wxDataViewItem & item, unsigned int col)
{
    wxVariant variant;
    variant << FbTitleData( m_title, not m_checked);
    model->ChangeValue(variant, item, col);
    return false;
}

// -----------------------------------------------------------------------------
// class FbBookModelData
// -----------------------------------------------------------------------------

#define BOOK_CASHE_SIZE 64

FbBookModelData::FbBookModelData(wxSQLite3ResultSet &result)
	: m_rowid(result.GetInt(0))
{
	m_title = result.GetString(1);
	m_values.Add(result.GetString(0));
	m_values.Add(result.GetString(1));
	m_values.Add(result.GetString(2));
}

FbBookModelData::FbBookModelData(const FbBookModelData &data)
    : m_rowid(data.m_rowid), m_values(data.m_values)
{
}

wxString FbBookModelData::GetValue(unsigned int col)
{
	return col && col <= m_values.GetCount() ? m_values[(int)col-1] : (wxString)wxEmptyString;
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
    wxSQLite3ResultSet result = m_database.ExecuteQuery(wxT("SELECT COUNT(id) FROM books"));
    return result.NextRow() ? result.GetInt(0) : 0;
}

FbBookModelData FbBookModelCashe::FindRow(unsigned int rowid)
{
	if ( m_rowid <= rowid && rowid < m_rowid + GetCount() ) return operator[]( rowid - m_rowid );

	m_rowid = rowid <= BOOK_CASHE_SIZE ? 1 : rowid - BOOK_CASHE_SIZE;

	Empty();
	wxString sql = wxT("SELECT rowid, title, file_size FROM books WHERE rowid>=? ORDER BY 1 LIMIT ?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, (wxLongLong)m_rowid);
	stmt.Bind(2, BOOK_CASHE_SIZE * 2);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FbBookModelData * found = NULL;
	unsigned int id = rowid;
	while (result.NextRow()) {
		FbBookModelData * data = new FbBookModelData(result);
		while ( id < data->Id() ) {
			Add(new FbBookModelData(id++));
		}
		if (id == rowid) found = data;
		Add(data);
		id++;
	}

	if (found) return *found;

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

