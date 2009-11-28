#include "FbAuthorThread.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"

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

void FbAuthorThread::FillAuthors(wxSQLite3ResultSet &result)
{
	if (sm_skiper.Skipped(m_number)) return;
	FbCommandEvent(fbEVT_AUTHOR_ACTION, ID_EMPTY_AUTHORS).Post(m_frame);
	while (result.NextRow()) {
		FbAuthorEvent(ID_APPEND_AUTHOR, result).Post(m_frame);
		if (sm_skiper.Skipped(m_number)) return;
	}
}

wxString FbAuthorThread::GetSQL(const wxString & condition)
{
	return wxString::Format( wxT("SELECT id, full_name, number FROM authors WHERE %sORDER BY search_name"), condition.c_str());
}

void FbAuthorThreadChar::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("letter=?"));
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, (wxString)m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

void FbAuthorThreadText::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("search_name like ?"));
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, BookInfo::MakeLower(m_text) + wxT("%"));
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

void FbAuthorThreadCode::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("id=?"));
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}
