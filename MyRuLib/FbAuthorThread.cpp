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

wxString FbAuthorThread::GetOrder()
{
	switch (m_order) {
		case -2: return wxT("number desc, name desc");
		case -1: return wxT("name desc");
		case  2: return wxT("number, name");
		default: return wxT("name ");
	}
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
	return wxString::Format( wxT("SELECT id, full_name as name, number FROM authors WHERE %s ORDER BY ") + GetOrder(), condition.c_str());
}

void FbAuthorThreadChar::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("letter=?"));
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_letter);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

void FbAuthorThreadText::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("SEARCH(search_name)"));
	FbSearchFunction search(m_mask);
	database.CreateFunction(wxT("SEARCH"), 1, search);
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
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

void FbAuthorThreadLast::GetResult(wxSQLite3Database &database)
{
	wxString sql = GetSQL(wxT("last_name=?"));
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_last);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	FillAuthors(result);
}

