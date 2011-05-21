#include "FbFrameThread.h"
#include <wx/filename.h>
#include "FbBookEvent.h"
#include "FbConst.h"

void FbFrameThread::AttachCounter(wxSQLite3Database &database, const wxString &filename)
{
	wxSQLite3Statement stmt = database.PrepareStatement(wxT("ATTACH ? AS cnt"));
	stmt.Bind(1, filename);
	stmt.ExecuteUpdate();
}

int FbFrameThread::GetCount(wxSQLite3Database &database, int code)
{
	if (!database.IsOpen()) return wxNOT_FOUND;
	wxString sql = wxT("SELECT num FROM numb WHERE key="); sql << code;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	return result.NextRow() ? result.GetInt(0) : 0;
}

void FbFrameThread::CreateCounter(wxSQLite3Database &database, const wxString &sql)
{
	if (!m_counter.IsEmpty()) return ;

	m_counter = wxFileName::CreateTempFileName(wxT("fb"));
	AttachCounter(database, m_counter);

	database.ExecuteUpdate(wxT("CREATE TABLE cnt.numb(key INTEGER PRIMARY KEY, num INTEGER)"));
	database.ExecuteUpdate(wxT("INSERT INTO cnt.numb(key, num)") + sql);

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

