#include "FbUpdateThread.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbExtractInfo.h"
#include "FbCounter.h"

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
	database.ExecuteUpdate(sql);
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
	ctx.SetResult(-id);
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
	FbCounter counter(database);
	counter.Add(m_sel);

	if (FbParams::GetValue(FB_REMOVE_FILES)) DoDelete(database);

	wxString sql;

	sql = wxString::Format(wxT("DELETE FROM books WHERE id IN (%s)"), m_sel.c_str());
	database.ExecuteUpdate(sql);

	sql = wxString::Format(wxT("DELETE FROM bookseq WHERE id_book IN (%s)"), m_sel.c_str());
	database.ExecuteUpdate(sql);

	sql = wxString::Format(wxT("DELETE FROM files WHERE id_book IN (%s)"), m_sel.c_str());
	database.ExecuteUpdate(sql);

	sql = wxString::Format(wxT("DELETE FROM genres WHERE id_book IN (%s)"), m_sel.c_str());
	database.ExecuteUpdate(sql);

	counter.Execute();

	return NULL;
}

void FbDeleteThread::DoDelete(FbDatabase &database)
{
	wxString basepath = FbParams::GetText(DB_LIBRARY_DIR);
	wxString sql = wxString::Format(wxT("SELECT id FROM books WHERE books.id IN (%s)"), m_sel.c_str());
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		FbExtractArray(database, result.GetInt(0)).DeleteFiles(basepath);
	}
}

void * FbTextThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	Sleep(2000);

	FbCommonDatabase().CreateFullText();

	return NULL;
}
