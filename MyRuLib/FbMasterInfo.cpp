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
		wxString sql = GetSQL();
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

wxString FbMasterAuthorInfo::GetSQL() const
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

wxString FbMasterSeqnInfo::GetSQL() const
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

wxString FbMasterGenrInfo::GetSQL() const
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

wxString FbMasterDateInfo::GetSQL() const
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

wxString FbMasterDownInfo::GetSQL() const
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

wxString FbMasterFldrInfo::GetSQL() const
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

wxString FbMasterSearchInfo::GetSQL() const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

void FbMasterSearchInfo::Bind(wxSQLite3Statement &stmt) const
{
//	stmt.Bind(1, m_id);
}

