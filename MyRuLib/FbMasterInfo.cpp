#include "FbMasterInfo.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbBookPanel.h"
#include "FbAuthList.h"
#include "FbDateTree.h"
#include "FbDownList.h"
#include "FbGenrTree.h"
#include "FbFldrTree.h"
#include "FbSeqnList.h"
#include "FbConst.h"

class FbMasterAuthInfo: public FbMasterInfoBase
{
	public:
		FbMasterAuthInfo(int id)
			: m_id(id) {}
		FbMasterAuthInfo(const FbMasterAuthInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterAuthInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual wxString GetOrderTable() const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterAuthInfo);
};

class FbMasterSeqnInfo: public FbMasterInfoBase
{
	public:
		FbMasterSeqnInfo(int id)
			: m_id(id) {}
		FbMasterSeqnInfo(const FbMasterSeqnInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterSeqnInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual wxString GetTreeSQL(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
		virtual void MakeTree(wxEvtHandler *owner, FbThread * thread, wxSQLite3ResultSet &result) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterSeqnInfo);
};

class FbMasterGenrInfo: public FbMasterInfoBase
{
	public:
		FbMasterGenrInfo(const wxString &id)
			: m_id(id) {}
		FbMasterGenrInfo(const FbMasterGenrInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterGenrInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		const wxString m_id;
		DECLARE_CLASS(FbMasterGenrInfo);
};

class FbMasterDownInfo: public FbMasterInfoBase
{
	public:
		FbMasterDownInfo(int id)
			: m_id(id) {}
		FbMasterDownInfo(const FbMasterDownInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterDownInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterDownInfo);
};

class FbMasterDateInfo: public FbMasterInfoBase
{
	public:
		FbMasterDateInfo(int id, int lib_min, int lib_max, int usr_min, int usr_max)
			: m_id(id), m_lib_min(lib_min), m_lib_max(lib_max), m_usr_min(usr_min), m_usr_max(usr_max) {}
		FbMasterDateInfo(const FbMasterDateInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id), m_lib_min(info.m_lib_min), m_lib_max(info.m_lib_max), m_usr_min(info.m_usr_min), m_usr_max(info.m_usr_max) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterDateInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		int m_lib_min;
		int m_lib_max;
		int m_usr_min;
		int m_usr_max;
		DECLARE_CLASS(FbMasterDateInfo);
};

class FbMasterFldrInfo: public FbMasterInfoBase
{
	public:
		FbMasterFldrInfo(int id)
			: m_id(id) {}
		FbMasterFldrInfo(const FbMasterFldrInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterFldrInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterFldrInfo);
};

class FbMasterCommInfo: public FbMasterInfoBase
{
	public:
		FbMasterCommInfo()
			{}
		FbMasterCommInfo(const FbMasterCommInfo &info)
			: FbMasterInfoBase(info) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterCommInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		DECLARE_CLASS(FbMasterCommInfo);
};

class FbMasterRateInfo: public FbMasterInfoBase
{
	public:
		FbMasterRateInfo(int id)
			: m_id(id) {}
		FbMasterRateInfo(const FbMasterRateInfo &info)
			: FbMasterInfoBase(info), m_id(info.m_id) {}
		virtual FbMasterInfoBase * Clone() const
			{ return new FbMasterRateInfo(*this); }
	protected:
		virtual wxString GetWhere(wxSQLite3Database &database) const;
		virtual void Bind(wxSQLite3Statement &stmt) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterRateInfo);
};

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

//-----------------------------------------------------------------------------
//  FbMasterDateInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbDateDayData::GetInfo() const
{
	return new FbMasterDateInfo(this->GetCode(), m_lib_min, m_lib_max, m_usr_min, m_usr_max);
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
	return new FbMasterAuthInfo(this->GetCode());
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
	return wxT("SELECT DISTINCT id_seq, books.id, number FROM books LEFT JOIN bookseq ON bookseq.id_book=books.id WHERE books.id_author=? ORDER BY id_seq, number, title");
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

	if (thread->IsClosed()) delete model;
	else FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
}

//-----------------------------------------------------------------------------
//  FbMasterSeqnInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbSeqnListData::GetInfo() const
{
	return new FbMasterSeqnInfo(this->GetCode());
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

	if (thread->IsClosed()) delete model;
	else FbModelEvent(ID_MODEL_CREATE, model, GetIndex()).Post(owner);
}

wxString FbMasterSeqnInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.id IN (SELECT id_book FROM bookseq WHERE bookseq.id_seq=?)");
}

wxString FbMasterSeqnInfo::GetTreeSQL(wxSQLite3Database &database) const
{
	wxString sql = wxT("SELECT DISTINCT books.id_author, books.id, bookseq.number FROM bookseq INNER JOIN books ON bookseq.id_book=books.id LEFT JOIN authors ON authors.id=books.id_author WHERE %s ORDER BY (CASE WHEN books.id_author=0 THEN 0 ELSE 1 END), authors.search_name, books.id_author, bookseq.number, books.title");
	return FormatTreeSQL(sql, GetWhere(database));
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
	return new FbMasterGenrInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterGenrInfo, FbMasterInfoBase)

wxString FbMasterGenrInfo::GetWhere(wxSQLite3Database &database) const
{
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
	return new FbMasterDownInfo(this->GetCode());
}

IMPLEMENT_CLASS(FbMasterDownInfo, FbMasterInfoBase)

wxString FbMasterDownInfo::GetWhere(wxSQLite3Database &database) const
{
	return wxT("books.md5sum IN (SELECT md5sum FROM favorites WHERE id_folder=?)");
}

void FbMasterDownInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, m_id);
}

//-----------------------------------------------------------------------------
//  FbMasterCommInfo
//-----------------------------------------------------------------------------

FbMasterInfo FbCommChildData::GetInfo() const
{
	return new FbMasterCommInfo();
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
	return new FbMasterRateInfo(this->GetCode());
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
	return new FbMasterFldrInfo(this->GetCode());
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

wxString FbMasterFindInfo::GetOrderTable() const
{
	wxString result;
	if (!m_author.IsEmpty()) result << wxT("INNER JOIN authors ON books.id_author = authors.id");
	if (GetOrderIndex() == BF_RATE) result << wxT("LEFT JOIN states ON books.md5sum=states.md5sum");
	return result;
}

wxString FbMasterFindInfo::GetWhere(wxSQLite3Database &database) const
{
	if (m_full) {
		wxString sql = wxT("books.id IN (SELECT docid FROM fts_book WHERE fts_book MATCH ?)");
		if (m_auth) sql << wxT("AND books.id_author IN (SELECT docid FROM fts_auth WHERE fts_auth MATCH ?)");
		return sql;
	} else {
		wxString sql = wxT("SEARCH_T(books.title)");
		if (m_auth) sql << wxT("AND SEARCH_A(authors.search_name)");
		return sql;
	}
}

void FbMasterFindInfo::Bind(wxSQLite3Statement &stmt) const
{
	stmt.Bind(1, FbSearchFunction::AddAsterisk(m_title));
	if (!m_author.IsEmpty()) stmt.Bind(2, FbSearchFunction::AddAsterisk(m_author));
}

void * FbMasterFindInfo::Execute(wxEvtHandler * owner, FbThread * thread)
{
	if (thread->IsClosed()) return NULL;
	try {
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
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}


