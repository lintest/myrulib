#include "FbUpdateThread.h"
#include "FbConst.h"


wxCriticalSection FbUpdateThread::sm_queue;

void * FbUpdateThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	database.AttachConfig();

	ExecSQL(database, m_sql);
	if (!m_sql2.IsEmpty()) ExecSQL(database, m_sql2);

	return NULL;
}

void FbUpdateThread::ExecSQL(FbDatabase &database, const wxString &sql)
{
	try {
		database.ExecuteUpdate(sql);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

class FbIncrementFunction : public wxSQLite3ScalarFunction
{
	public:
		FbIncrementFunction(): m_increment(0) {};
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		int m_increment;
};

void FbIncrementFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int id = ctx.GetArgCount()>0 ? ctx.GetInt(0) : 0;
	m_increment++;
	id += m_increment;
	ctx.SetResult(id);
}

void * FbFolderUpdateThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	database.AttachConfig();

	ExecSQL(database, m_sql);
	if (!m_sql2.IsEmpty()) ExecSQL(database, m_sql2);

	FbFolderEvent(ID_UPDATE_FOLDER, m_folder, m_type).Post();

	return NULL;
}

void * FbCreateDownloadThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	FbIncrementFunction function;
	database.CreateFunction(wxT("INCREMENT"), 1, function);
	database.AttachConfig();

	ExecSQL(database, m_sql);
	if (!m_sql2.IsEmpty()) ExecSQL(database, m_sql2);

	FbFolderEvent(ID_UPDATE_FOLDER, m_folder, m_type).Post();

	return NULL;
}

void * FbDeleteThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	wxString sql;

	sql = wxString::Format(wxT("books.id IN (%s)"), m_sel.c_str());
	LogDelete(database, sql);

	sql = wxString::Format(wxT("DELETE FROM books WHERE id IN (%s)"), m_sel.c_str());
	ExecSQL(database, sql);

	sql = wxString::Format(wxT("DELETE FROM bookseq WHERE id_book IN (%s)"), m_sel.c_str());
	ExecSQL(database, sql);

	sql = wxString::Format(wxT("DELETE FROM files WHERE id_book IN (%s)"), m_sel.c_str());
	ExecSQL(database, sql);

	ExecSQL(database, strUpdateAuthorCount);
	ExecSQL(database, strUpdateSequenCount);

	return NULL;
}

void FbUpdateThread::LogDelete(FbDatabase &database, const wxString &where)
{
	wxString sql = wxT("SELECT books.file_name, archives.file_name FROM books LEFT JOIN archives ON archives.id=books.id_archive WHERE ") + where;
	try {
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		while (result.NextRow()) {
			wxString file = result.GetString(0);
			wxString zip = result.GetString(1);
			wxString msg = wxT("Delete: ");
			if (!zip.IsEmpty()) msg += zip + wxT(": ");
			msg += file;
			wxLogInfo(msg);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}
