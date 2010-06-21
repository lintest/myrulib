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
	if (thread->TestDestroy()) return NULL;

	try {
		FbCommonDatabase database;
		FbGenreFunction func_genre;
		FbAggregateFunction func_aggregate;
		database.CreateFunction(wxT("AGGREGATE"), 1, func_aggregate);
		database.CreateFunction(wxT("GENRE"), 1, func_genre);
		if (GetOrderIndex() == BF_RATE) database.AttachConfig();
		if (thread->TestDestroy()) return NULL;

		wxString sql;
		switch (GetMode()) {
			case FB2_MODE_LIST: sql = GetListSQL(database); break;
			case FB2_MODE_TREE: sql = GetTreeSQL(database); break;
		}

		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		Bind(stmt);
		if (thread->TestDestroy()) return NULL;

		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (thread->TestDestroy()) return NULL;

		switch (GetMode()) {
			case FB2_MODE_LIST: MakeList(owner, thread, result); break;
			case FB2_MODE_TREE: MakeTree(owner, thread, result); break;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbMasterInfo::MakeList(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (thread->TestDestroy()) return;
		items.Add(result.GetInt(0));
		count++;
		if (count == length) {
			length = fbLIST_ARRAY_SIZE;
			FbArrayEvent(id, items, GetIndex()).Post(owner);
			id = ID_MODEL_APPEND;
			items.Empty();
			count = 0;
		}
	}
	FbArrayEvent(id, items, GetIndex()).Post(owner);
}

void FbMasterInfo::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	FbTreeModel * model = new FbTreeModel;
	FbParentData * root = new FbParentData(*model, NULL);
	FbAuthParentData * auth = NULL;
	FbAuthParentData * list = NULL;
	FbSeqnParentData * seqn = NULL;

	while (result.NextRow()) {
		if (thread->TestDestroy()) return;
		int auth_id = result.GetInt(0);
		int seqn_id = result.GetInt(1);
		if (auth == NULL) {
			auth = new FbAuthParentData(*model, NULL, auth_id);
			list = new FbAuthParentData(*model, NULL, auth_id);
		}
		if (seqn == NULL) {
			seqn = new FbSeqnParentData(*model, auth, seqn_id);
		}
		if (auth->GetCode() != auth_id) {
			if (auth->Count(*model) == 1 && seqn->GetCode() == 0) {
				delete auth;
				list->SetParent(root);
			} else {
				auth->SetParent(root);
				auth->SortItems();
				delete list;
			}
			auth = new FbAuthParentData(*model, NULL, auth_id);
			list = new FbAuthParentData(*model, NULL, auth_id);
			seqn = new FbSeqnParentData(*model, auth, seqn_id);
		}
		if (seqn->GetCode() != seqn_id) {
			seqn = new FbSeqnParentData(*model, auth, seqn_id);
		}
		new FbBookChildData(*model, seqn, result.GetInt(2), result.GetInt(3));
		new FbBookChildData(*model, list, result.GetInt(2), result.GetInt(3));
	}

	if (auth->Count(*model) == 1 && seqn->GetCode() == 0) {
		delete auth;
		list->SetParent(root);
	} else {
		auth->SetParent(root);
		auth->SortItems();
		delete list;
	}

	model->SetRoot(root);

	FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
}

wxString FbMasterInfo::GetOrderTable() const
{
	switch (GetOrderIndex()) {
		case BF_AUTH: return wxT("LEFT JOIN authors ON books.id_author = authors.id");
		case BF_RATE: return wxT("LEFT JOIN states ON books.md5sum=states.md5sum");
		default: return wxEmptyString;
	}
}

wxString FbMasterInfo::GetOrderColumn() const
{
	switch (GetOrderIndex()) {
		case BF_NAME: return wxT("books.title");
		case BF_NUMB: return wxT("books.title");
		case BF_AUTH: return wxT("full_name");
		case BF_CODE: return wxT("books.code");
		case BF_GENR: return wxT("genre_list");
		case BF_RATE: return wxT("states.rating");
		case BF_LANG: return wxT("books.lang");
		case BF_TYPE: return wxT("books.file_type");
		case BF_DATE: return wxT("books.created");
		case BF_SIZE: return wxT("books.file_size");
		case BF_BITE: return wxT("books.file_size");
		case BF_SEQN: return wxT("books.title");
		case BF_MD5S: return wxT("books.md5sum");
		default: return wxT("books.title");
	}
}

wxString FbMasterInfo::GetSelectColumn() const
{
	switch (GetOrderIndex()) {
		case BF_AUTH: return wxT(",AGGREGATE(authors.full_name) AS full_name");
		case BF_GENR: return wxT(",GENRE(books.genres) AS genre_list");
		default: return wxEmptyString;
	}
}

wxString FbMasterInfo::GetOrderFields() const
{
	wxString column = GetOrderColumn();
	wxString result = wxT(" ORDER BY ");
	result << column;
	if (m_order < 0) result << wxT(" DESC");
	if (column != wxT("books.title")) {
		result << wxT(',') << wxT("books.title");
		if (m_order < 0) result << wxT(" DESC");
	}
	return result;
}

wxString FbMasterInfo::GetListSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT books.id %s FROM books %s WHERE %s GROUP BY books.id %s");
}

wxString FbMasterInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT books.id_author, bookseq.id_seq, books.id, bookseq.number FROM books LEFT JOIN authors ON authors.id=books.id_author LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE %s ORDER BY (CASE WHEN books.id_author=0 THEN 0 ELSE 1 END), authors.search_name, books.id_author, bookseq.id_seq, bookseq.number, books.title");
}

//-----------------------------------------------------------------------------
//  FbMasterAuthorInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterAuthorInfo, FbMasterInfo)

wxString FbMasterAuthorInfo::GetOrderTable() const
{
	if (GetOrderIndex() == BF_AUTH) 
		return wxT("LEFT JOIN books AS b ON b.id=books.id LEFT JOIN authors ON b.id_author = authors.id");
	else 
		return FbMasterInfo::GetOrderTable();
}

wxString FbMasterAuthorInfo::GetListSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT books.id %s FROM books %s WHERE books.id_author=? GROUP BY books.id %s");
	return wxString::Format(sql, GetSelectColumn().c_str(), GetOrderTable().c_str(), GetOrderFields().c_str());
}

wxString FbMasterAuthorInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
}

void FbMasterAuthorInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

void FbMasterAuthorInfo::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
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

	FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
}

//-----------------------------------------------------------------------------
//  FbMasterDateInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterDateInfo, FbMasterInfo)

wxString FbMasterDateInfo::GetListSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT books.id %s FROM books %s WHERE books.created=? GROUP BY books.id %s");
	return wxString::Format(sql, GetSelectColumn().c_str(), GetOrderTable().c_str(), GetOrderFields().c_str());
}

wxString FbMasterDateInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString sql = FbMasterInfo::GetTreeSQL(database);
	return wxString::Format(sql, wxT("books.created=?"));
}

void FbMasterDateInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterSeqnInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterSeqnInfo, FbMasterInfo)

void FbMasterSeqnInfo::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
}

wxString FbMasterSeqnInfo::GetListSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id_book FROM bookseq LEFT JOIN books on books.id = bookseq.id_book WHERE bookseq.id_seq=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_author, books.id, number FROM bookseq LEFT JOIN books ON bookseq.id_book=books.id WHERE bookseq.id_seq=? ORDER BY id_author, number, title");
	}
}

wxString FbMasterSeqnInfo::GetTreeSQL(wxSQLite3Database &database) const
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

wxString FbMasterGenrInfo::GetListSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT id_book %s FROM genres INNER JOIN books on books.id = genres.id_book %s WHERE genres.id_genre=? %s");
	return wxString::Format(sql, GetSelectColumn().c_str(), GetOrderTable().c_str(), GetOrderFields().c_str());
}

wxString FbMasterGenrInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString sql = FbMasterInfo::GetTreeSQL(database);
	return wxString::Format(sql, wxT("books.id IN (SELECT id_book FROM genres WHERE genres.id_genre=?)"));
}

void FbMasterGenrInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterDownInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterDownInfo, FbMasterInfo)

wxString FbMasterDownInfo::GetListSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT id_book %s FROM genres INNER JOIN books on books.id = genres.id_book %s WHERE genres.id_genre=? %s");
	return wxString::Format(sql, GetSelectColumn().c_str(), GetOrderTable().c_str(), GetOrderFields().c_str());
}

wxString FbMasterDownInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString sql = FbMasterInfo::GetTreeSQL(database);
	return wxString::Format(sql, wxT("books.id IN (SELECT id_book FROM genres WHERE genres.id_genre=?)"));
}

void FbMasterDownInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterFldrInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterFldrInfo, FbMasterInfo)

wxString FbMasterFldrInfo::GetListSQL(wxSQLite3Database &database) const
{
	switch (GetMode()) {
		case FB2_MODE_LIST:
			return wxT("SELECT DISTINCT id FROM books WHERE id_author=? ORDER BY title");
		case FB2_MODE_TREE:
			return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
	}
}

wxString FbMasterFldrInfo::GetTreeSQL(wxSQLite3Database &database) const
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

wxString FbMasterSearchInfo::GetListSQL(wxSQLite3Database &database) const
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

wxString FbMasterSearchInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString condition;
	wxString sql = FbMasterInfo::GetTreeSQL(database);
	return wxString::Format(sql, condition.c_str());
}

void FbMasterSearchInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_title));
	if (!m_author.IsEmpty()) stmt.Bind(2, FbSearchFunction::AddAsterisk(m_author));
}


