#include "FbMasterTypes.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbBookPanel.h"
#include "FbAuthList.h"
#include "FbDateTree.h"
#include "FbDownList.h"
#include "FbGenrTree.h"
#include "FbFldrTree.h"
#include "FbSeqnList.h"
#include "FbGenres.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbMasterDateInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbDateDayData::GetInfo() const
{
	return FbMasterDateInfo(this->GetCode(), m_lib_min, m_lib_max, m_usr_min, m_usr_max);
}

IMPLEMENT_CLASS(FbMasterDateInfo, FbMasterInfoBase)

wxString FbMasterDateInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("((books.id BETWEEN ? AND ?)OR(books.id BETWEEN ? AND ?))AND(books.created=?)");
}

void FbMasterDateInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_lib_min);
	stmt.Bind(2, m_lib_max);
	stmt.Bind(3, m_usr_min);
	stmt.Bind(4, m_usr_max);
	stmt.Bind(5, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterAuthInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbAuthListData::GetInfo() const
{
	return FbMasterAuthInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterAuthInfo, FbMasterInfoBase)

wxString FbMasterAuthInfo::GetOrderTable() const
{
	if (GetOrderIndex() == BF_AUTH)
		return wxT("LEFT JOIN books AS b ON b.id=books.id LEFT JOIN authors ON b.id_author = authors.id");
	else
		return FbMasterInfoBase::GetOrderTable();
}

wxString FbMasterAuthInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.id_author=?");
}

wxString FbMasterAuthInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT id_seq, books.id, bookseq.number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id %s WHERE %s ORDER BY id_seq, %s");
}

void FbMasterAuthInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

void FbMasterAuthInfo::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	FbBookTreeModel * model = new FbBookTreeModel;
	FbAuthParentData * root = new FbAuthParentData(*model, NULL, m_id);
	FbAuthParentData * author = new FbAuthParentData(*model, NULL, m_id);
	FbSeqnParentData * parent = NULL;

	while (result.NextRow()) {
		if (thread->IsClosed()) break;
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
	SendTree(owner, thread, model);
}

//-----------------------------------------------------------------------------
//  FbMasterSeqnInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbSeqnListData::GetInfo() const
{
	return FbMasterSeqnInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterSeqnInfo, FbMasterInfoBase)

void FbMasterSeqnInfo::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	FbBookTreeModel * model = new FbBookTreeModel;
	FbSeqnParentData * root = new FbSeqnParentData(*model, NULL, m_id);
	FbAuthParentData * parent = NULL;

	while (result.NextRow()) {
		if (thread->IsClosed()) break;
		int auth = result.GetInt(0);
		if (parent == NULL || parent->GetCode() != auth) {
			parent = new FbAuthParentData(*model, root, auth);
		}
		new FbBookChildData(*model, parent, result.GetInt(1), result.GetInt(2));
	}
	model->SetRoot(root);
	SendTree(owner, thread, model);
}

wxString FbMasterSeqnInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.id IN (SELECT id_book FROM bookseq WHERE bookseq.id_seq=?)");
}

wxString FbMasterSeqnInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT books.id_author, books.id, bookseq.number FROM bookseq INNER JOIN books ON bookseq.id_book=books.id LEFT JOIN authors ON authors.id=books.id_author %s WHERE %s ORDER BY (CASE WHEN books.id_author=0 THEN 0 ELSE 1 END), authors.search_name, books.id_author, %s");
}

void FbMasterSeqnInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterGenrInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbGenrChildData::GetInfo() const
{
	return FbMasterGenrInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterGenrInfo, FbMasterInfoBase)

wxString FbMasterGenrInfo::GetWhere(wxSQLite3Database &database) const
{
	if (m_id.IsEmpty()) return wxT("books.genres=?");
	return wxT("books.id IN (SELECT id_book FROM genres WHERE genres.id_genre=?)");
}

void FbMasterGenrInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterDownInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbDownListData::GetInfo() const
{
	return FbMasterDownInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterDownInfo, FbMasterInfoBase)

wxString FbMasterDownInfo::GetWhere(wxSQLite3Database &database) const
{
	wxString sql = wxT("books.md5sum IN (SELECT md5sum FROM states WHERE download");
	switch (m_id) {
		case DT_WAIT:  sql << wxT("<0)"); break;
		case DT_ERROR: sql << wxT("=1)"); break;
		case DT_READY: sql << wxT(">1)"); break;
	}
	return sql;
}

void FbMasterDownInfo::Bind(wxSQLite3Statement &stmt) const
{
}

//-----------------------------------------------------------------------------
//  FbMasterCommInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbCommChildData::GetInfo() const
{
	return FbMasterCommInfo();
}

IMPLEMENT_CLASS(FbMasterCommInfo, FbMasterInfoBase)

wxString FbMasterCommInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.md5sum IN (SELECT DISTINCT md5sum FROM comments)");
}

void FbMasterCommInfo::Bind(wxSQLite3Statement &stmt) const
{
}

//-----------------------------------------------------------------------------
//  FbMasterRateInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbRateChildData::GetInfo() const
{
	return FbMasterRateInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterRateInfo, FbMasterInfoBase)

wxString FbMasterRateInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.md5sum IN (SELECT md5sum FROM states WHERE rating=?)");
}

void FbMasterRateInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterFldrInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbFolderChildData::GetInfo() const
{
	return FbMasterFldrInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterFldrInfo, FbMasterInfoBase)

wxString FbMasterFldrInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.md5sum IN (SELECT md5sum FROM favorites WHERE id_folder=?)");
}

void FbMasterFldrInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterFindInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterFindInfo, FbMasterInfoBase)

wxString FbMasterFindInfo::GetWhere(wxSQLite3Database &database) const
{
	if (m_full) {
		wxString sql = wxT("books.id IN (SELECT docid FROM fts_book WHERE fts_book MATCH ?)");
		if (m_auth) sql << wxT("AND books.id_author IN (SELECT docid FROM fts_auth WHERE fts_auth MATCH ?)");
		return sql;
	} else {
		wxString sql = wxT("SEARCH_T(books.title)");
		if (m_auth) sql << wxT("AND books.id_author IN (SELECT id FROM authors WHERE SEARCH_A(authors.search_name))");
		return sql;
	}
}

void FbMasterFindInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_title));
	if (!m_author.IsEmpty()) stmt.Bind(2, FbSearchFunction::AddAsterisk(m_author));
}

void * FbMasterFindInfo::Execute(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter)
{
	if (thread->IsClosed()) return NULL;

	FbCommonDatabase database;
	FbGenreFunction func_genre;
	FbAggregateFunction func_aggregate;
	FbSearchFunction func_title(m_title);
	FbSearchFunction func_author(m_author);
	database.CreateFunction(wxT("AGGREGATE"), 1, func_aggregate);
	database.CreateFunction(wxT("GENRE"), 1, func_genre);
	database.AttachConfig();
	if (thread->IsClosed()) return NULL;

	m_auth = !m_author.IsEmpty();
	m_full = database.TableExists(wxT("fts_book"));
	m_full &= FbSearchFunction::IsFullText(m_title);
	m_full &= FbSearchFunction::IsFullText(m_author);

	wxString sql;
	switch (GetMode()) {
		case FB2_MODE_LIST: sql = GetListSQL(database); break;
		case FB2_MODE_TREE: sql = GetTreeSQL(database); break;
	}
	sql = FormatSQL(sql, GetWhere(database), filter);

	if (!m_full) {
		database.CreateFunction(wxT("SEARCH_T"), 1, func_title);
		if (m_auth) database.CreateFunction(wxT("SEARCH_A"), 1, func_author);
	}

	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	if (m_full) Bind(stmt);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (thread->IsClosed()) return NULL;

	switch (GetMode()) {
		case FB2_MODE_LIST: MakeList(owner, thread, result); break;
		case FB2_MODE_TREE: MakeTree(owner, thread, result); break;
	}

	return NULL;
}
