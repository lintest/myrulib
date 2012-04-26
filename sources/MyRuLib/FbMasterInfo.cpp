#include "FbMasterInfo.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include "models/FbBookList.h"
#include "models/FbBookTree.h"
#include "controls/FbTreeView.h"
#include "FbGenres.h"

//-----------------------------------------------------------------------------
//  FbMasterInfoBase
//-----------------------------------------------------------------------------

int FbMasterInfoBase::sm_counter = 0;

IMPLEMENT_CLASS(FbMasterInfoBase, wxObject)

void * FbMasterInfoBase::Execute(wxEvtHandler * owner, FbThread * thread, const FbFilterObj &filter)
{
	if (thread->IsClosed()) return NULL;

	FbCommonDatabase database;
	database.JoinThread(thread);
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
	sql = FormatSQL(sql, GetWhere(database), filter);

	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	Bind(stmt);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (!result.IsOk()) return NULL;

	switch (GetMode()) {
		case FB2_MODE_LIST: MakeList(owner, thread, result); break;
		case FB2_MODE_TREE: MakeTree(owner, thread, result); break;
	}
	return NULL;
}

void FbMasterInfoBase::MakeList(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const
{
	wxWindowID id = ID_MODEL_CREATE;
	size_t length = fbLIST_CACHE_SIZE;
	size_t index = 0;
	size_t count = 0;
	wxArrayInt items;
	while (result.NextRow()) {
		if (thread->IsClosed()) return;
		items.Add(result.GetInt(0));
		count++;
		index++;
		if (index == length) {
			length = fbLIST_ARRAY_SIZE;
			FbArrayEvent(id, items, GetIndex()).Post(owner);
			id = ID_MODEL_APPEND;
			items.Empty();
			index = 0;
		}
	}
	FbArrayEvent(id, items, GetIndex()).Post(owner);
	FbCountEvent(ID_BOOKS_COUNT, *this, count).Post(owner);
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
	SendTree(owner, thread, model);
}

void FbMasterInfoBase::SendTree(wxEvtHandler *owner, FbThread * thread, FbBookTreeModel * model) const
{
	if (thread->IsClosed()) {
		delete model;
	} else {
		int count = model->GetBookCount();
		FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
		FbCountEvent(ID_BOOKS_COUNT, *this, count).Post(owner);
	}
}

wxString FbMasterInfoBase::GetOrderTable() const
{
	switch (GetOrderIndex()) {
		case BF_AUTH: {
			if (GetMode ()== FB2_MODE_LIST)
				return wxT("LEFT JOIN authors ON books.id_author = authors.id");
			else return wxEmptyString;
		} break;
		case BF_NUMB: {
			if (GetMode ()== FB2_MODE_LIST)
				return wxT("LEFT JOIN bookseq ON bookseq.id_book = books.id");
			else return wxEmptyString;
		} break;
		case BF_SEQN: {
			if (GetMode ()== FB2_MODE_LIST)
				return wxT("LEFT JOIN bookseq ON bookseq.id_book = books.id LEFT JOIN sequences ON sequences.id=bookseq.id_seq");
			else
				return wxT("LEFT JOIN sequences ON sequences.id=bookseq.id_seq");
		} break;
		case BF_RATE:
		case BF_DOWN:
			return wxT("LEFT JOIN states ON books.md5sum=states.md5sum");
		default:
			return wxEmptyString;
	}
}

wxString FbMasterInfoBase::GetOrderColumn() const
{
	if (GetMode() == FB2_MODE_LIST) {
		switch (GetOrderIndex()) {
			case BF_NUMB: return wxT("MAX(bookseq.number)");
			case BF_AUTH: return wxT("AGGREGATE(authors.full_name)") + fbCOLLATE_CYR;
			case BF_SEQN: return wxT("AGGREGATE(value)") + fbCOLLATE_CYR;
			default: ;
		}
	}
	switch (GetOrderIndex()) {
		case BF_NUMB: return wxT("bookseq.number");
		case BF_AUTH: return wxT("authors.full_name") + fbCOLLATE_CYR;
		case BF_CODE: return wxT("books.id");
		case BF_GENR: return wxT("GENRE(books.genres)") + fbCOLLATE_CYR;
		case BF_RATE: return wxT("states.rating");
		case BF_LANG: return wxT("books.lang");
		case BF_TYPE: return wxT("books.file_type");
		case BF_DATE: return wxT("books.created");
		case BF_SIZE: return wxT("books.file_size");
		case BF_BITE: return wxT("books.file_size");
		case BF_SEQN: return wxT("value") + fbCOLLATE_CYR;
		case BF_DOWN: return wxT("states.download");
		case BF_MD5S: return wxT("books.md5sum");
		default: return wxT("books.title") + fbCOLLATE_CYR;
	}
}

wxString FbMasterInfoBase::GetOrderFields() const
{
	wxString title = wxT("books.title") + fbCOLLATE_CYR;
	wxString column = GetOrderColumn();
	wxString result = column;
	if (m_order < 0) result << wxT(" DESC");
	if (column != title) {
		result << wxT(',') << title;
		if (m_order < 0) result << wxT(" DESC");
	}
	return result;
}

wxString FbMasterInfoBase::GetListSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT books.id FROM books %s WHERE %s GROUP BY books.id ORDER BY %s");
}

wxString FbMasterInfoBase::GetTreeSQL(wxSQLite3Database &database) const
{
	return wxT("SELECT DISTINCT books.id_author, bookseq.id_seq, books.id, bookseq.number FROM books LEFT JOIN authors ON authors.id=books.id_author LEFT JOIN bookseq ON bookseq.id_book=books.id  %s WHERE %s ORDER BY (CASE WHEN books.id_author=0 THEN 0 ELSE 1 END), authors.full_name COLLATE CYR, books.id_author, bookseq.id_seq, %s");
}

wxString FbMasterInfoBase::FormatSQL(const wxString &sql, const wxString &cond, const FbFilterObj &filter) const
{
	wxString table = GetOrderTable();
	wxString fields = GetOrderFields();
	wxString where = cond;
	where << filter.GetFilterSQL();
	wxString result = wxString::Format(sql, table.c_str(), where.c_str(), fields.c_str());
	return result;
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

bool FbMasterInfo::operator ==(const FbMasterInfo &info) const
{
	if (this->m_data == NULL) return m_data == NULL;
	return (*this->m_data) == (*info.m_data);
}

FbMasterInfo FbModelData::GetInfo(FbModel & model) const
{
	return FbMasterInfo();
}

FbMasterInfo FbModelItem::GetInfo() const
{
	return m_data ? m_data->GetInfo(*m_model) : FbMasterInfo();
}

FbMasterInfo FbTreeViewCtrl::GetInfo() const
{
	FbModel * model = GetModel();
	if (model) {
		FbModelItem item = model->GetCurrent();
		if (item) return (&item)->GetInfo(*model);
	}
	return FbMasterInfo();
}


