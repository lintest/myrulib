#include "FbTreeModel.h"

// -----------------------------------------------------------------------------
// class FbTreeDataArray
// -----------------------------------------------------------------------------

WX_DEFINE_OBJARRAY(FbTreeDataArray);

// -----------------------------------------------------------------------------
// class FbLetterDataNode
// -----------------------------------------------------------------------------

void FbLetterDataNode::GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col)
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

bool FbLetterDataNode::SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        m_checked = data.IsChecked();
    }
    return true;
}

unsigned int FbLetterDataNode::GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children )
{
	if (m_children.Count()) {
		for (size_t i=0; i<m_children.Count(); i++) children.Add( wxDataViewItem(m_children[i]) );
		return m_children.Count();
	} else {
		for (size_t i=0; i<m_count; i++) {
			FbTreeDataNode * item = new FbAuthorDataNode(this, m_checked);
			m_children.Add( item );
			children.Add( wxDataViewItem(item) );
		}
		return m_count;
	}
}

void FbLetterDataNode::CheckChildren(wxSQLite3Database * database)
{
	if (m_count) {
		wxString sql = wxT("SELECT id FROM authors WHERE letter=? ORDER BY search_name");
		wxSQLite3Statement stmt = database->PrepareStatement(sql);
		stmt.Bind(1, (wxString)m_letter);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		size_t i = 0;
		while (result.NextRow()) {
			if (i>=m_children.Count()) break;
			FbAuthorDataNode * author = (FbAuthorDataNode*)(m_children[i]);
			author->SetId(result.GetInt(0));
			i++;
		}
		m_count = 0;
	}
}

// -----------------------------------------------------------------------------
// class FbAuthorDataNode
// -----------------------------------------------------------------------------

bool FbAuthorDataNode::SeqExists(wxSQLite3Database * database)
{
    wxString sql = wxT("SELECT id_seq FROM bookseq WHERE bookseq.id_author=? AND id_seq<>0 LIMIT 1");
    wxSQLite3Statement stmt = database->PrepareStatement(sql);
    stmt.Bind(1, m_id);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();
    return result.NextRow();
}

unsigned int FbAuthorDataNode::GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children )
{
	if (m_children.Count()) {
		for (size_t i=0; i<m_children.Count(); i++) children.Add( wxDataViewItem(m_children[i]) );
		return m_children.Count();
	};

	if (SeqExists(database)) {
        wxString sql = wxT("SELECT COUNT(DISTINCT id_seq) FROM bookseq WHERE bookseq.id_author=?");
        wxSQLite3Statement stmt = database->PrepareStatement(sql);
        stmt.Bind(1, m_id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        m_count = result.NextRow() ? result.GetInt(0) : 0;
		for (size_t i=0; i<m_count; i++) {
			FbTreeDataNode * item = new FbSequenceDataNode(this, m_checked);
			m_children.Add( item );
		    children.Add( wxDataViewItem(item) );
		}
	} else {
        wxString sql = wxT("SELECT COUNT(id_book) FROM bookseq WHERE bookseq.id_author=?");
        wxSQLite3Statement stmt = database->PrepareStatement(sql);
        stmt.Bind(1, m_id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        m_count = result.NextRow() ? result.GetInt(0) : 0;
		for (size_t i=0; i<m_count; i++) {
			FbTreeDataNode * item = new FbBookDataNode(this, m_checked);
			m_children.Add( item );
		    children.Add( wxDataViewItem(item) );
		}
    }
	return m_count;
}

void FbAuthorDataNode::GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col)
{
	m_owner->CheckChildren(database);
    switch ( col ) {
		case FbTreeModel::COL_TITLE: {
			wxString sql = wxT("SELECT full_name FROM authors WHERE id=?");
			wxSQLite3Statement stmt = database->PrepareStatement(sql);
			stmt.Bind(1, m_id);
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			wxString name = result.NextRow() ? result.GetString(0) : wxString();
			variant << FbTitleData( name, m_checked );
		} break;
		default: {
			variant = wxT("author");
		}
	}
}

bool FbAuthorDataNode::SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        m_checked = data.IsChecked();
    }
    return true;
}

void FbAuthorDataNode::CheckChildren(wxSQLite3Database * database)
{
	if (m_count) {
		wxString sql = wxT("SELECT id_seq, COUNT(id_book) FROM bookseq LEFT JOIN sequences ON sequences.id=bookseq.id_seq WHERE id_author=? GROUP BY id_seq ORDER BY sequences.value");
		wxSQLite3Statement stmt = database->PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		size_t i = 0;
		while (result.NextRow()) {
			if (i>=m_children.Count()) break;
			FbSequenceDataNode * item = (FbSequenceDataNode*)(m_children[i]);
			item->SetId(result.GetInt(0), result.GetInt(1));
			i++;
		}
		m_count = 0;
	}
}

void FbAuthorDataNode::CheckBooks(wxSQLite3Database * database)
{
	if (m_count) {
		wxString sql = wxT("SELECT id FROM books WHERE id_author=? ORDER BY title");
		wxSQLite3Statement stmt = database->PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		size_t i = 0;
		while (result.NextRow()) {
			if (i>=m_children.Count()) break;
			FbBookDataNode * item = (FbBookDataNode*)(m_children[i]);
			item->SetId(result.GetInt(0));
			i++;
		}
		m_count = 0;
	}
}

// -----------------------------------------------------------------------------
// class FbSequenceDataNode
// -----------------------------------------------------------------------------

wxString FbSequenceDataNode::GetName(wxSQLite3Database * database)
{
    if (m_id) {
        wxString sql = wxT("SELECT value FROM sequences WHERE id=?");
        wxSQLite3Statement stmt = database->PrepareStatement(sql);
        stmt.Bind(1, m_id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return result.GetString(0);
    }
    return wxT("(Misc.)");
}

void FbSequenceDataNode::GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col)
{
	m_owner->CheckChildren(database);
    switch ( col ) {
		case FbTreeModel::COL_TITLE: {
			variant << FbTitleData( GetName(database), m_checked );
		} break;
		default: {
			variant = wxT("sequence");
		}
	}
}

bool FbSequenceDataNode::SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col)
{
    if (col == FbTreeModel::COL_TITLE) {
        FbTitleData data;
        data << variant;
        m_checked = data.IsChecked();
    }
    return true;
}

unsigned int FbSequenceDataNode::GetChildren( wxSQLite3Database * database, wxDataViewItemArray &children )
{
	if (m_children.Count()) {
		for (size_t i=0; i<m_children.Count(); i++) children.Add( wxDataViewItem(m_children[i]) );
		return m_children.Count();
	} else {
		for (size_t i=0; i<m_count; i++) {
			FbTreeDataNode * item = new FbBookDataNode(this, m_checked);
			m_children.Add( item );
			children.Add( wxDataViewItem(item) );
		}
		return m_count;
	}
}

void FbSequenceDataNode::CheckChildren(wxSQLite3Database * database)
{
	if (m_count) {
		wxString sql = wxT("SELECT id FROM books WHERE id IN (SELECT id_book FROM bookseq INDEXED BY bookseq_author WHERE id_author=? AND id_seq=?) AND books.id_author=? ORDER BY books.title");
		wxSQLite3Statement stmt = database->PrepareStatement(sql);
		stmt.Bind(1, m_owner->GetId());
		stmt.Bind(2, m_id);
		stmt.Bind(3, m_owner->GetId());
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		size_t i = 0;
		while (result.NextRow()) {
			if (i>=m_children.Count()) break;
			FbBookDataNode * item = (FbBookDataNode*)(m_children[i]);
			item->SetId(result.GetInt(0));
			i++;
		}
		m_count = 0;
	}
}

// -----------------------------------------------------------------------------
// class FbBookDataNode
// -----------------------------------------------------------------------------

wxString FbBookDataNode::GetName(wxSQLite3Database * database)
{
    if (m_id) {
        wxString sql = wxT("SELECT title FROM books WHERE id=? LIMIT 1");
        wxSQLite3Statement stmt = database->PrepareStatement(sql);
        stmt.Bind(1, m_id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return result.GetString(0);
    }
    return wxT("(not found)");
}

void FbBookDataNode::GetValue(wxSQLite3Database * database, wxVariant &variant, unsigned int col)
{
    switch (m_owner->GetType()) {
        case NT_AUTHOR:
            ((FbAuthorDataNode*)m_owner)->CheckBooks(database);
            break;
        case NT_SEQUENCE:
            ((FbSequenceDataNode*)m_owner)->CheckChildren(database);
            break;
    }

    switch ( col ) {
		case FbTreeModel::COL_TITLE: {
			variant << FbTitleData( GetName(database), m_checked );
		} break;
		case FbTreeModel::COL_ROWID: {
			variant = wxString::Format("%d", m_id);
		} break;
	}
}

bool FbBookDataNode::SetValue(wxSQLite3Database * database, const wxVariant &variant, unsigned int col)
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
	if (item.IsOk()) ((FbTreeDataNode*)item.GetID())->GetValue(m_database, variant, col);
}

bool FbTreeModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col)
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->SetValue(m_database, variant, col) : false;
}

bool FbTreeModel::GetAttr(const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr) const
{
    if (item.IsOk() && IsContainer(item))  {
        attr.SetBold(true);
        return true;
    } else return false;
}

wxDataViewItem FbTreeModel::GetParent( const wxDataViewItem &item ) const
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->GetParent() : wxDataViewItem(NULL);
}

bool FbTreeModel::IsContainer( const wxDataViewItem &item ) const
{
	return item.IsOk() ? ((FbTreeDataNode*)item.GetID())->IsContainer() : true;
}

