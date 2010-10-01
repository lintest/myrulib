#include "FbUpdateThread.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbExtractInfo.h"
#include "FbCounter.h"
#include "MyRuLibApp.h"

wxCriticalSection FbUpdateThread::sm_queue;

void * FbUpdateThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	database.AttachConfig();

	if (!m_sql1.IsEmpty()) ExecSQL(database, m_sql1);
	if (!m_sql2.IsEmpty()) ExecSQL(database, m_sql2);
	if (!m_sql3.IsEmpty()) ExecSQL(database, m_sql3);

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

	ExecSQL(database, m_sql1);
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

	ExecSQL(database, m_sql1);
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

	if (FbParams::GetInt(FB_REMOVE_FILES)) DoDelete(database);

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
	wxString basepath = wxGetApp().GetLibPath();
	wxString sql = wxString::Format(wxT("SELECT id FROM books WHERE books.id IN (%s)"), m_sel.c_str());
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		FbExtractArray(database, result.GetInt(0)).DeleteFiles(basepath);
	}
}

void * FbGenreThread::Entry()
{
	FbCommonDatabase database;
	wxSQLite3Transaction trans(&database, WXSQLITE_TRANSACTION_EXCLUSIVE);

	wxString msg = _("Rebuild the list of genres");

	database.ExecuteUpdate(wxT("DROP TABLE genres"));
	database.ExecuteUpdate(wxT("CREATE TABLE genres(id_book integer, id_genre CHAR(2));"));
	database.ExecuteUpdate(wxT("CREATE INDEX genres_book ON genres(id_book);"));
  	database.ExecuteUpdate(wxT("CREATE INDEX genres_genre ON genres(id_genre);"));

	int count = database.ExecuteScalar(wxT("SELECT count(DISTINCT id) FROM books"));
	if (count == 0) return NULL;

	int pos = 0;
	wxString sql = wxT("SELECT id, genres FROM books");
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		int id = res.GetInt(0);
		wxString genres = res.GetString(1);
		int len = genres.Len();
		for (int i = 0; i < len; i+=2) {
			wxString sql = wxT("INSERT INTO genres(id_book, id_genre) VALUES(?,?)");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, id);
			stmt.Bind(2, genres.Mid(i, 2));
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
		}
		FbProgressEvent(ID_PROGRESS_UPDATE, msg, ++pos * 1000 / count).Post();
	}

	trans.Commit();
	FbProgressEvent(ID_PROGRESS_UPDATE).Post();

	return NULL;
}


