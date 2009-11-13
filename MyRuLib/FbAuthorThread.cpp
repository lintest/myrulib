#include "FbAuthorThread.h"
#include "FbConst.h"
#include "FbDatabase.h"

wxCriticalSection FbAuthorThread::sm_queue;

FbThreadSkiper FbAuthorThread::sm_skiper;

void * FbAuthorThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;

	try {
		FbCommonDatabase database;
		GetResult(database);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbAuthorThread::Execute()
{
	if ( Create() == wxTHREAD_NO_ERROR ) Run();
}

void FbAuthorThread::FillAuthors(wxSQLite3ResultSet &result)
{
	if (sm_skiper.Skipped(m_number)) return;
	FbCommandEvent(fbEVT_AUTHOR_ACTION, ID_EMPTY_AUTHORS).Post(m_frame);
	while (result.NextRow()) {
		FbAuthorEvent(ID_APPEND_AUTHOR, result).Post(m_frame);
		if (sm_skiper.Skipped(m_number)) return;
	}
}

void FbAuthorThreadChar::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("\
		SELECT authors.id as id, full_name, search_name, COUNT(books.id) AS number FROM \
		(SELECT id, full_name, search_name FROM authors WHERE letter=?) AS authors \
		LEFT JOIN books ON authors.id = books.id_author \
		GROUP BY authors.id, full_name, search_name \
		HAVING COUNT(books.id)>0 \
		ORDER BY search_name \
	");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, (wxString)m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

void FbAuthorThreadText::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE search_name like ? ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_text + wxT("%"));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

void FbAuthorThreadCode::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}
