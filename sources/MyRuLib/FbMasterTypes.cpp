#include "FbMasterTypes.h"
#include "models/FbBookList.h"
#include "models/FbBookTree.h"
#include "models/FbAuthList.h"
#include "models/FbClssTree.h"
#include "models/FbDateTree.h"
#include "models/FbDownList.h"
#include "models/FbFldrTree.h"
#include "models/FbGenrTree.h"
#include "models/FbSeqnList.h"
#include "FbBookPanel.h"
#include "FbGenres.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbMasterDateInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbDateDayData::GetInfo(FbModel & model) const
{
	return FbMasterDateInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterDateInfo, FbMasterInfoBase)

wxString FbMasterDateInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.created=?");
}

void FbMasterDateInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterAuthInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbAuthListData::GetInfo(FbModel & model) const
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

FbMasterInfo FbSeqnListData::GetInfo(FbModel & model) const
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

FbMasterInfo FbGenrChildData::GetInfo(FbModel & model) const
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

FbMasterInfo FbDownListData::GetInfo(FbModel & model) const
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

FbMasterInfo FbCommChildData::GetInfo(FbModel & model) const
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

FbMasterInfo FbRateChildData::GetInfo(FbModel & model) const
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

FbMasterInfo FbFolderChildData::GetInfo(FbModel & model) const
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
	if (!DoFind(owner, thread, filter)) {
		FbCommandEvent event(fbEVT_BOOK_ACTION, ID_FOUND_NOTHING, m_title);
		wxWindow * frame = ((wxWindow*)owner)->GetParent();
		event.SetEventObject(frame);
		event.Post();
	}
	return NULL;
}

bool FbMasterFindInfo::DoFind(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter)
{
	if (thread->IsClosed()) return false;

	FbCommonDatabase database;
	database.JoinThread(thread);
	FbGenreFunction func_genre;
	FbAggregateFunction func_aggregate;
	FbSearchFunction func_title(m_title);
	FbSearchFunction func_author(m_author);
	database.CreateFunction(wxT("AGGREGATE"), 1, func_aggregate);
	database.CreateFunction(wxT("GENRE"), 1, func_genre);
	database.AttachConfig();
	if (thread->IsClosed()) return false;

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
	if (!result.IsOk()) return false;

	bool ok = !result.Eof();

	switch (GetMode()) {
		case FB2_MODE_LIST: MakeList(owner, thread, result); break;
		case FB2_MODE_TREE: MakeTree(owner, thread, result); break;
	}

	return ok;
}

//-----------------------------------------------------------------------------
//  FbMasterClssInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbClssModelData::GetInfo(FbModel & model) const
{
	FbClssTreeModel & owner = (FbClssTreeModel&) model;
	return FbMasterClssInfo(owner.GetBookSQL(), this->GetCode());
}

IMPLEMENT_CLASS(FbMasterClssInfo, FbMasterInfoBase)

wxString FbMasterClssInfo::GetWhere(wxSQLite3Database &database) const
{
	return m_sql;
}

void FbMasterClssInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}
