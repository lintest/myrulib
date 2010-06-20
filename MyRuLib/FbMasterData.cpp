#include "FbMasterData.h"
#include "FbFrameBase.h"
#include "FbBookEvent.h"
#include "FbBookList.h"
#include "FbBookTree.h"
#include "FbConst.h"

FbMasterData::BaseThread::BaseThread(FbFrameBase * frame, FbMasterData const * data) :
	m_frame(frame),
	m_mode(frame->GetListMode()),
	m_filter(frame->GetFilterSQL()),
	m_order(frame->GetOrderSQL())
{
	frame->SetMasterData(data);
}

wxCriticalSection FbMasterData::BaseThread::sm_queue;

wxString FbMasterData::BaseThread::GetSQL(const wxString & condition)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE:
			sql = wxT("\
				SELECT DISTINCT (CASE WHEN bookseq.id_seq IS NULL THEN 1 WHEN bookseq.id_seq=0 THEN 1 ELSE 0 END) AS key, \
					books.id, books.id_author, books.title, books.file_size, books.file_type, books.lang, GENRE(books.genres) AS genres,\
					states.rating, books.id_author, authors.full_name, sequences.value AS sequence, bookseq.number\
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id  \
					LEFT JOIN bookseq ON bookseq.id_book=books.id \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				ORDER BY \
			") + GetOrder();
			break;
		case FB2_MODE_LIST:
			sql = wxT("\
				SELECT DISTINCT \
					books.id, books.title, books.file_size, books.file_type, books.lang, GENRE(books.genres) AS genres,\
					states.rating as rating, books.created, AGGREGATE(authors.full_name) as full_name \
				FROM books \
					LEFT JOIN authors ON books.id_author = authors.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				GROUP BY books.id, books.title, books.file_size, books.file_type, states.rating, books.created \
				ORDER BY \
			") + GetOrder();
			break;
	}

	sql += FbParams::GetLimit();

	wxString str = wxString::Format(wxT("(%s)%s"), condition.c_str(), m_filter.c_str());
	return wxString::Format(sql, str.c_str());
}

wxString FbMasterData::BaseThread::GetOrder()
{
	if (m_mode == FB2_MODE_TREE)
		return  wxT("authors.search_name, key, sequences.value, bookseq.number, books.title");
	else
		return m_order;
}

void FbMasterData::BaseThread::CreateTree(wxSQLite3ResultSet &result)
{
	bool seq_exists = false;
	wxString thisAuthor = wxT("@@@");
	wxString thisSequence = wxT("@@@");
	while (result.NextRow()) {
		int id_author = result.GetInt(wxT("id_author"));
		wxString nextAuthor = result.GetString(wxT("full_name"));
		wxString nextSequence = result.GetString(wxT("sequence"));

		if (thisAuthor != nextAuthor) {
			thisAuthor = nextAuthor;
			thisSequence = wxT("@@@");
			seq_exists = false;
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR, id_author, nextAuthor).Post(m_frame);
		}
		if (thisSequence != nextSequence) {
			thisSequence = nextSequence;
			seq_exists |= result.GetInt(wxT("key")) == 0;
			if (seq_exists) FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE, thisSequence).Post(m_frame);
		}
		BookTreeItemData data(result);
		FbBookEvent(ID_APPEND_BOOK, &data).Post(m_frame);
	}
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_BOOKS_COUNT).Post(m_frame);
}

void FbMasterData::BaseThread::CreateList(wxSQLite3ResultSet &result)
{
	while (result.NextRow()) {
		BookTreeItemData data(result);
		wxString full_name = result.GetString(wxT("full_name"));
		FbBookEvent(ID_APPEND_BOOK, &data, full_name).Post(m_frame);
	}
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_BOOKS_COUNT).Post(m_frame);
}

void FbMasterData::BaseThread::EmptyBooks()
{
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_EMPTY_BOOKS).Post(m_frame);
}

void FbMasterData::BaseThread::FillBooks(wxSQLite3ResultSet &result)
{
	switch (m_mode) {
		case FB2_MODE_TREE: CreateTree(result); break;
		case FB2_MODE_LIST: CreateList(result); break;
	}
}

void FbMasterData::BaseThread::InitDatabase(FbCommonDatabase &database)
{
	database.AttachConfig();
	database.CreateFunction(wxT("AGGREGATE"), 1, m_aggregate);
	database.CreateFunction(wxT("GENRE"), 1, m_genre);
}

FbThreadSkiper FbMasterDownld::DownldThread::sm_skiper;

wxString FbMasterDownld::DownldThread::GetOrder()
{
	if (m_folder==1) return wxT("download");
	return BaseThread::GetOrder();
}

void * FbMasterDownld::DownldThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition;
	condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE download");
	condition += ( m_folder==1 ? wxT(">=?)") : wxT("=?)") );

	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_folder);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

FbThreadSkiper FbMasterFolder::FolderThread::sm_skiper;

void * FbMasterFolder::FolderThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition;

	switch (m_type) {
		case FT_FOLDER:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM favorites WHERE id_folder=?)");
			break;
		case FT_RATING:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE rating=?)");
			break;
		case FT_COMMENT:
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM comments WHERE ?>0)");
			break;
		case FT_DOWNLOAD: {
			condition = wxT("books.md5sum IN(SELECT DISTINCT md5sum FROM states WHERE download");
			condition += ( m_folder==1 ? wxT(">=?)") : wxT("=?)") );
			} break;
		default:{
			condition = wxT("books.md5sum = ?");
			} break;
	}
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_folder);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

FbMasterGenre::SubgenreFunction::SubgenreFunction(int code)
	: m_code( wxString::Format(wxT("%02X"), code) )
{
}

void FbMasterGenre::SubgenreFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(wxT("SUBGENRE called with wrong number of arguments: %d."), argCount));
		return;
	}
	wxString text = ctx.GetString(0);

	for (size_t i=0; i<text.Length()/2; i++) {
		if ( text.Mid(i*2, 2) == m_code ) {
			ctx.SetResult(true);
			return;
		}
	}
	ctx.SetResult(false);
}

FbThreadSkiper FbMasterGenre::GenresThread::sm_skiper;

void * FbMasterGenre::GenresThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		if (database.TableExists(wxT("genres"))) {
			wxString condition = wxT("books.id IN (SELECT id_book FROM genres WHERE id_genre=?)");
			wxString sql = GetSQL(condition);
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, m_subgenre.GetCode());
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			if (sm_skiper.Skipped(m_number)) return NULL;
			FillBooks(result);
		} else {
			wxString condition = wxT("SUBGENRE(books.genres)");
			wxString sql = GetSQL(condition);
			database.CreateFunction(wxT("SUBGENRE"), 1, m_subgenre);
			wxSQLite3ResultSet result = database.ExecuteQuery(sql);
			if (sm_skiper.Skipped(m_number)) return NULL;
			FillBooks(result);
		}
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void * FbMasterSearch::SearchThread::Entry()
{

	wxCriticalSectionLocker locker(sm_queue);

	EmptyBooks();
	bool bUseAuthor = !m_author.IsEmpty();
	bool bFullText = FbSearchFunction::IsFullText(m_title) && FbSearchFunction::IsFullText(m_author);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		if ( bFullText && database.TableExists(wxT("fts_book")) ) {
			wxString condition = wxT("books.id IN (SELECT docid FROM fts_book WHERE fts_book MATCH ?)");
			if (bUseAuthor) condition += wxT("AND books.id_author IN (SELECT docid FROM fts_auth WHERE fts_auth MATCH ?)");
			wxString sql = GetSQL(condition);

			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, FbSearchFunction::AddAsterisk(m_title));
			if (bUseAuthor) stmt.Bind(2, FbSearchFunction::AddAsterisk(m_author));

			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			if (result.Eof()) {
				FbCommandEvent(fbEVT_BOOK_ACTION, ID_FOUND_NOTHING).Post(m_frame);
				return NULL;
			}
			FillBooks(result);
		} else {
			wxString condition = wxT("SEARCH_T(books.title)");
			if (bUseAuthor) condition += wxT("AND SEARCH_A(authors.search_name)");
			wxString sql = GetSQL(condition);

			FbSearchFunction sfTitle(m_title);
			FbSearchFunction sfAuthor(m_author);
			database.CreateFunction(wxT("SEARCH_T"), 1, sfTitle);
			if (bUseAuthor) database.CreateFunction(wxT("SEARCH_A"), 1, sfAuthor);
			wxSQLite3Statement stmt = database.PrepareStatement(sql);

			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			if (result.Eof()) {
				FbCommandEvent(fbEVT_BOOK_ACTION, ID_FOUND_NOTHING).Post(m_frame);
				return NULL;
			}
			FillBooks(result);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

FbThreadSkiper FbMasterSeqname::SequenThread::sm_skiper;

void * FbMasterSeqname::SequenThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	try {
		FbCommonDatabase database;
		InitDatabase(database);

		wxString condition = wxT("books.id IN (SELECT id_book FROM bookseq WHERE id_seq=?)");
		if (m_mode == FB2_MODE_TREE) condition += wxT("AND bookseq.id_seq=?");
		wxString sql = GetSQL(condition);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_master);
		if (m_mode == FB2_MODE_TREE) stmt.Bind(2, m_master);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

FbThreadSkiper FbMasterDate::DateThread::sm_skiper;

void * FbMasterDate::DateThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition = wxT("created=?");
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_master);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbMasterSeqname::SequenThread::CreateTree(wxSQLite3ResultSet &result)
{
	wxString thisSequence = wxT("@@@");
	wxString thisAuthor = wxT("@@@");
	while (result.NextRow()) {
		int id_author = result.GetInt(wxT("id_author"));
		wxString nextSequence = result.GetString(wxT("sequence"));
		wxString nextAuthor = result.GetString(wxT("full_name"));

		if (thisSequence != nextSequence) {
			thisSequence = nextSequence;
			thisAuthor = wxT("@@@");
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE, thisSequence).Post(m_frame);
		}
		if (thisAuthor != nextAuthor) {
			thisAuthor = nextAuthor;
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR, id_author, nextAuthor).Post(m_frame);
		}
		BookTreeItemData data(result);
		FbBookEvent(ID_APPEND_BOOK, &data).Post(m_frame);
	}
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_BOOKS_COUNT).Post(m_frame);
}

wxString FbMasterSeqname::SequenThread::GetOrder()
{
	if (m_mode == FB2_MODE_TREE)
		return  wxT("sequences.value, authors.search_name, key, bookseq.number, books.title");
	else
		return m_order;
}

void FbMasterAuthor::Show(FbFrameBase * frame) const
{
	if (frame->GetListMode() == FB2_MODE_TREE) {
		(new FbBookTreeThread(frame->GetBooks(), m_id))->Execute();
	} else {
		(new FbBookListThread(frame->GetBooks(), m_id))->Execute();
	}
}

void FbMasterDownld::Show(FbFrameBase * frame) const
{
	(new DownldThread(frame, this))->Execute();
}

void FbMasterFolder::Show(FbFrameBase * frame) const
{
	(new FolderThread(frame, this))->Execute();
}

void FbMasterGenre::Show(FbFrameBase * frame) const
{
	(new GenresThread(frame, this))->Execute();
}

void FbMasterSearch::Show(FbFrameBase * frame) const
{
	(new SearchThread(frame, this))->Execute();
}

void FbMasterSeqname::Show(FbFrameBase * frame) const
{
	(new SequenThread(frame, this))->Execute();
}

void FbMasterDate::Show(FbFrameBase * frame) const
{
	(new DateThread(frame, this))->Execute();
}

wxDateTime FbMasterDate::GetDate(int code)
{
    int m = (code / 100 % 100 - 1);
    if (m >= 12) m = 0;
    wxDateTime::Month month = wxDateTime::Month(m + wxDateTime::Jan);
	wxDateTime date = wxDateTime::Today();
    date.SetYear(code / 10000 + 2000);
    date.SetMonth(month);
    date.SetDay(code % 100);
    return date;
}


