#include "FbTreeModel.h"

// -----------------------------------------------------------------------------
// class FbTreeModelArray
// -----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbTreeModelArray);

// -----------------------------------------------------------------------------
// class FbLetterDataNode
// -----------------------------------------------------------------------------

void FbLetterDataNode::GetValue(wxVariant &variant, unsigned int col)
{
    switch ( col ) {
		case FbTreeModel::COL_TITLE: {
			variant << FbTitleData( m_letter, m_checked );
		} break;
		default: {
			variant = wxString(m_letter);
		}
	}
}

bool FbLetterDataNode::SetValue(const wxVariant &variant, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        m_checked = data.IsChecked();
    }
    return true;
}

bool FbLetterDataNode::GetAttr(unsigned int col, wxDataViewItemAttr &attr)
{
    attr.SetBold(true);
    return true;
}

unsigned int FbLetterDataNode::GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children )
{
	if (m_children.Count()) {
		for (size_t i=0; i<m_children.Count(); i++) children.Add( wxDataViewItem(m_children[i]) );
		return m_children.Count();
	} else {
		for (size_t i=0; i<m_count; i++) {
			FbAuthorDataNode * item = new FbAuthorDataNode(this);
			m_children.Add( item );
			children.Add( wxDataViewItem(item) );
		}
		return m_count;
	}
}

// -----------------------------------------------------------------------------
// class FbAuthorDataNode
// -----------------------------------------------------------------------------

void FbAuthorDataNode::GetValue(wxVariant &variant, unsigned int col)
{
    switch ( col ) {
		case FbTreeModel::COL_TITLE: {
			variant << FbTitleData( wxT("author"), m_checked );
		} break;
		default: {
			variant = wxT("author");
		}
	}
}

bool FbAuthorDataNode::SetValue(const wxVariant &variant, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        m_checked = data.IsChecked();
    }
    return true;
}

// -----------------------------------------------------------------------------
// class FbTreeModelData
// -----------------------------------------------------------------------------

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
// class FbTreeModel
// -----------------------------------------------------------------------------

FbTreeModel::FbTreeModel(const wxString &filename)
{
	m_database = new wxSQLite3Database;
	m_database->Open(filename);

	wxString sql = wxT("SELECT letter, count(id) FROM authors GROUP BY letter ORDER BY 1");
	wxSQLite3ResultSet result = m_database->ExecuteQuery(sql);
	while (result.NextRow()) {
		wxString text = result.GetString(0);
		wxChar letter = text.IsEmpty() ? wxT(' ') : text[0];
		unsigned int count = result.GetInt(1);
		FbLetterDataNode * item = new FbLetterDataNode(letter, count);
		m_children.Add(item);
	}
}

FbTreeModel::~FbTreeModel()
{
    wxDELETE(m_database);
}

unsigned int FbTreeModel::GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const
{
	if (item.IsOk()) {
		return ((FbTreeDataNode*)item.GetID())->GetChildren(m_database, children);
	} else {
		for (size_t i=0; i<m_children.Count(); i++) children.Add( wxDataViewItem(m_children[i]) );
		return m_children.Count();
	}
}

void FbTreeModel::GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const
{
	if (item.IsOk()) ((FbTreeDataNode*)item.GetID())->GetValue(variant, col);
}

bool FbTreeModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col)
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->SetValue(variant, col) : false;
}

virtual bool FbTreeModel::GetAttr(const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr) const
{
    return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->GetAttr(col, attr) : false;
}

wxDataViewItem FbTreeModel::GetParent( const wxDataViewItem &item ) const
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->GetParent() : wxDataViewItem(NULL);
}

bool FbTreeModel::IsContainer( const wxDataViewItem &item ) const
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->IsContainer() : true;
}

