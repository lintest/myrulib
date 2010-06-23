#include "FbMasterInfo.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbBookList.h"
#include "FbBookTree.h"

//-----------------------------------------------------------------------------
//  FbMasterInfoBase
//-----------------------------------------------------------------------------

int FbMasterInfoBase::sm_counter = 0;

IMPLEMENT_CLASS(FbMasterInfoBase, wxObject)

void * FbMasterInfoBase::Execute(wxEvtHandler * owner, FbThread * thread)
{
	if (thread->IsClosed()) return NULL;
	try {
		FbCommonDatabase database;
		FbGenreFunction func_genre;
		FbAggregateFunction func_aggregate;
		database.CreateFunction(wxT("AGGREGATE"), 1, func_aggregate);
		database.CreateFunction(wxT("GENRE"), 1, func_genre);
		database.AttachConfig();
		if (thread->IsClosed()) return NULL;

		wxString sql;
		switch (GetMode()) {
			case FB2_MODE_LIST: sql = GetListSQL(database); break;
			case FB2_MODE_TREE: sql = GetTreeSQL(database); break;
		}

		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		Bind(stmt);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (thread->IsClosed()) return NULL;
		switch (GetMode()) {
			case FB2_MODE_LIST: MakeList(owner, thread, result); break;
			case FB2_MODE_TREE: MakeTree(owner, thread, result); break;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbMasterInfoBase::MakeList(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (thread->IsClosed()) return;
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

void FbMasterInfoBase::MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	FbBookTreeModel * model = new FbBookTreeModel;
	FbParentData * root = new FbParentData(*model, NULL);
	FbAuthParentData * auth = NULL;
	FbAuthParentData * list = NULL;
	FbSeqnParentData * seqn = NULL;

	while (result.NextRow()) {
		if (thread->IsClosed()) break;
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

	if (auth && list) {
		if (auth->Count(*model) == 1 && seqn->GetCode() == 0) {
			delete auth;
			list->SetParent(root);
		} else {
			auth->SetParent(root);
			auth->SortItems();
			delete list;
		}
	}

	model->SetRoot(root);

	if (thread->IsClosed()) delete model;
	else FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
}

wxString FbMasterInfoBase::GetOrderTable() const
{
	switch (GetOrderIndex()) {
		case BF_AUTH: return wxT("LEFT JOIN authors ON books.id_author = authors.id");
		case BF_RATE: return wxT("LEFT JOIN states ON books.md5sum=states.md5sum");
		default: return wxEmptyString;
	}
}

wxString FbMasterInfoBase::GetOrderColumn() const
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

wxString FbMasterInfoBase::GetSelectColumn() const
{
	switch (GetOrderIndex()) {
		case BF_AUTH: return wxT(",AGGREGATE(authors.full_name) AS full_name");
		case BF_GENR: return wxT(",GENRE(books.genres) AS genre_list");
		default: return wxEmptyString;
	}
}

wxString FbMasterInfoBase::GetOrderFields() const
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

wxString FbMasterInfoBase::GetListSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT books.id %s FROM books %s WHERE %s GROUP BY books.id %s");
	return FormatListSQL(sql, GetWhere(database));
}

wxString FbMasterInfoBase::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT books.id_author, bookseq.id_seq, books.id, bookseq.number FROM books LEFT JOIN authors ON authors.id=books.id_author LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE %s ORDER BY (CASE WHEN books.id_author=0 THEN 0 ELSE 1 END), authors.search_name, books.id_author, bookseq.id_seq, bookseq.number, books.title");
	return FormatTreeSQL(sql, GetWhere(database));
}

wxString FbMasterInfoBase::FormatListSQL(const wxString &sql, const wxString &cond) const
{
	return wxString::Format(sql, GetSelectColumn().c_str(), GetOrderTable().c_str(), cond.c_str(), GetOrderFields().c_str());
}

wxString FbMasterInfoBase::FormatTreeSQL(const wxString &sql, const wxString &cond) const
{
	return wxString::Format(sql, cond.c_str());
}

//-----------------------------------------------------------------------------
//  FbMasterInfo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterInfo, wxObject)

FbMasterInfo & FbMasterInfo::operator =(const FbMasterInfo &info)
{
	wxDELETE(m_data);
	if (info.m_data) m_data = info.m_data->Clone();
	return *this;
}

FbMasterInfo FbModelData::GetInfo() const
{
	return NULL;
}

FbMasterInfo FbModelItem::GetInfo() const
{
	return m_data ? m_data->GetInfo() : FbMasterInfo();
}

