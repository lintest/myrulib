#include "FbFrameThread.h"
#include <wx/filename.h>
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbFrameThread
//-----------------------------------------------------------------------------

FbFrameDatabase::FbFrameDatabase(FbThread * thread, wxString &filename)
{
	bool create = filename.IsEmpty();
	if (create) filename = wxFileName::CreateTempFileName(wxT("fb"));
	int flags = create ? (WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_READWRITE) : WXSQLITE_OPEN_READONLY;
	Open(filename, wxEmptyString, flags);
	JoinThread(thread);
	AttachCommon();
}

//-----------------------------------------------------------------------------
//  FbFrameThread
//-----------------------------------------------------------------------------

int FbFrameThread::GetCount(FbSQLite3Database &database, int code)
{
	if (!database.IsOpen()) return wxNOT_FOUND;
	wxString sql = wxT("SELECT num FROM numb WHERE key="); sql << code;
	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	return result.NextRow() ? result.GetInt(0) : 0;
}

void FbFrameThread::SetCountSQL(const wxString &sql, const FbFilterObj &filter)
{
	m_sql = wxString::Format(sql, filter.GetFilterSQL().c_str());
}

void FbFrameThread::CreateCounter(FbSQLite3Database &database, const wxString &sql)
{
	if (IsClosed()) return;
	database.ExecuteUpdate(wxT("CREATE TABLE numb(key INTEGER PRIMARY KEY, num INTEGER)"));
	database.ExecuteUpdate(wxT("INSERT INTO numb(key, num)") + m_sql);
	if (IsClosed()) {
		database.Close();
		wxRemoveFile(m_counter);
		m_counter = wxEmptyString;
	} else {
		FbCommandEvent(fbEVT_BOOK_ACTION, ID_MODEL_NUMBER, m_counter).Post(m_frame);
	}
}

wxString FbFrameThread::GetOrder(int order, const wxString &standart)
{
	wxString sql = standart;
	if (abs(order) > 1) sql = wxT("numb.num");
	if (order < 0) sql << wxT(" desc");
	sql.Prepend(wxT(" ORDER BY "));
	return sql;
}

//-----------------------------------------------------------------------------
//  FbCountThread
//-----------------------------------------------------------------------------

void * FbCountThread::Entry()
{
	if (m_sql.IsEmpty()) return NULL;
	FbFrameDatabase database(this, m_counter);
	CreateCounter(database, m_sql);
	return NULL;
}
