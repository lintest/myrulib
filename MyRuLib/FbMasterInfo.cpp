#include "FbMasterInfo.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbBookPanel.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbMasterInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterInfo, wxObject)

void * FbMasterInfo::Execute(wxEvtHandler * owner, FbThread * thread) const
{
	try {
		FbCommonDatabase database;
		wxString sql = GetSQL(database);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		Bind(stmt);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		switch (GetMode()) {
			case FB2_MODE_LIST: MakeList(owner, result); break;
			case FB2_MODE_TREE: MakeTree(owner, result); break;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbMasterInfo::MakeList(wxEvtHandler *owner, wxSQLite3ResultSet &result) const
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		items.Add(result.GetInt(0));
		count++;
		if (count == length) {
			length = fbLIST_ARRAY_SIZE;
			FbArrayEvent(id, items).Post(owner);
			id = ID_MODEL_APPEND;
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(id, items).Post(owner);
}

void FbMasterInfo::MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const
{
}

//-----------------------------------------------------------------------------
//  FbMasterAuthorInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterAuthorInfo, FbMasterInfo)

wxString FbMasterAuthorInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterAuthorInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

void FbMasterAuthorInfo::MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const
{
	FbTreeModel * model = new FbTreeModel;
	FbAuthParentData * root = new FbAuthParentData(*model, NULL, m_id);
	FbAuthParentData * author = new FbAuthParentData(*model, NULL, m_id);
	FbSeqnParentData * parent = NULL;

	while (result.NextRow()) {
		int seqn = result.GetInt(0);
		if (parent == NULL || parent->GetCode() != seqn) {
			parent = new FbSeqnParentData(*model, root, seqn);
		}
		new FbBookChildData(*model, parent, result.GetInt(1), result.GetInt(2));
		new FbBookChildData(*model, author, result.GetInt(1), result.GetInt(2));
	}

	if (root->Count(*model) == 1 && parent && parent->GetCode() == 0) {
		wxDELETE(root);
		root = author;
	} else {
		wxDELETE(author);
		author = root;
		root->SortItems();
	}
	model->SetRoot(root);

	FbModelEvent(ID_BOOKS_LISTCTRL, model).Post(owner);
}

//-----------------------------------------------------------------------------
//  FbMasterSeqnInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterSeqnInfo, FbMasterInfo)

void FbMasterSeqnInfo::MakeTree(wxEvtHandler *owner, wxSQLite3ResultSet &result) const
{
}

wxString FbMasterSeqnInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id_book FROM bookseq LEFT JOIN books on books.id = bookseq.id_book WHERE bookseq.id_seq=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_author, books.id, number FROM bookseq LEFT JOIN books ON bookseq.id_book=books.id WHERE bookseq.id_seq=? ORDER BY id_author, number, title");
	}
}

void FbMasterSeqnInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterGenrInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterGenrInfo, FbMasterInfo)

wxString FbMasterGenrInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id_book FROM genres INNER JOIN books on books.id = genres.id_book WHERE genres.id_genre=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterGenrInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterDateInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterDateInfo, FbMasterInfo)

wxString FbMasterDateInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE created=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterDateInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterDownInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterDownInfo, FbMasterInfo)

wxString FbMasterDownInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterDownInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterFldrInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterFldrInfo, FbMasterInfo)

wxString FbMasterFldrInfo::GetSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterFldrInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterSearchInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterSearchInfo, FbMasterInfo)

wxString FbMasterSearchInfo::GetSQL(wxSQLite3Database &database) const
{
	bool auth = !m_author.IsEmpty();
	wxString sql = wxT("SELECT DISTINCT books.id FROM books WHERE ");
	sql << wxT("books.id IN (SELECT docid FROM fts_book WHERE fts_book MATCH ?)");
	if (auth) sql << wxT("AND books.id_author IN (SELECT docid FROM fts_auth WHERE fts_auth MATCH ?)");
	return sql << wxT("ORDER BY title");
/*
	bool full = FbSearchFunction::IsFullText(m_title) && FbSearchFunction::IsFullText(m_author);
	bool auth = !m_author.IsEmpty();

	if ( full && database.TableExists(wxT("fts_book")) ) {
		switch (GetMode()) {
			case FB2_MODE_LIST: {
				wxString sql = wxT("SELECT DISTINCT books.id FROM books WHERE ");
				sql << wxT("books.id IN (SELECT docid FROM fts_book WHERE fts_book MATCH ?)");
				if (auth) sql << wxT("AND books.id_author IN (SELECT docid FROM fts_auth WHERE fts_auth MATCH ?)");
				return sql << wxT("ORDER BY title");
			} break;
			case FB2_MODE_TREE: {
				return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
			} break;
		}
	} else {
		case FB2_MODE_LIST: {
			wxString sql = wxT("SELECT DISTINCT books.id FROM books WHERE ");
			sql << wxT("SEARCH_T(books.title)");
			if (auth) sql << wxT("AND SEARCH_A(authors.search_name)");
			return sql << wxT("ORDER BY title");
			FbSearchFunction sfTitle(m_title);
			FbSearchFunction sfAuthor(m_author);
			database.CreateFunction(wxT("SEARCH_T"), 1, sfTitle);
			if (bUseAuthor) database.CreateFunction(wxT("SEARCH_A"), 1, sfAuthor);
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
		} break;
			case FB2_MODE_TREE: {
				return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
			} break;
		}
	}
*/
}

void FbMasterSearchInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_title));
	if (!m_author.IsEmpty()) stmt.Bind(2, FbSearchFunction::AddAsterisk(m_author));
}

