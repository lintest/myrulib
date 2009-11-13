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
		wxSQLite3ResultSet result = GetResult(database);
		if (sm_skiper.Skipped(m_number)) return NULL;
		FbCommandEvent(fbEVT_AUTHOR_ACTION, ID_EMPTY_AUTHORS).Post(m_frame);
		while (result.NextRow()) {
			FbAuthorEvent( ID_APPEND_AUTHOR, result).Post(m_frame);
			if (sm_skiper.Skipped(m_number)) return NULL;
		}
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

wxSQLite3ResultSet FbAuthorThreadChar::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE letter=? ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, (wxString)m_letter);
	return stmt.ExecuteQuery();
}

wxSQLite3ResultSet FbAuthorThreadText::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE search_name like ? ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_text + wxT("%"));
	return stmt.ExecuteQuery();
}

wxSQLite3ResultSet FbAuthorThreadCode::GetResult(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_code);
	return stmt.ExecuteQuery();
}
