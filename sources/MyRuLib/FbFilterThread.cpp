#include "FbFilterThread.h"
#include "FbDatabase.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include <wx/filename.h>

void * FbFilterThread::Entry()
{
	int result = Execute() ? wxID_OK : wxID_CANCEL;
	FbCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, result, m_tempfile).Post(m_owner);
	return NULL;
}

bool FbFilterThread::Execute()
{
	m_tempfile = wxFileName::CreateTempFileName(wxT("fb"));
	wxRemoveFile(m_tempfile);
	FbCommonDatabase database;

	FbProgressEvent(ID_PROGRESS_PULSE, _("Create filter")).Post(m_owner);

	wxSQLite3Statement stmt = database.PrepareStatement(wxT("ATTACH ? AS f"));
	stmt.Bind(1, m_tempfile);
	stmt.ExecuteUpdate();

	wxSQLite3Transaction trans(&database);
	database.JoinThread(this);

	wxArrayString sql;

	sql.Add(wxT("CREATE TEMP TABLE tb(b INTEGER NOT NULL, a INTEGER, d INTEGER, PRIMARY KEY(b, a))"));
	sql.Add(wxT("CREATE TABLE f.fa(a INTEGER PRIMARY KEY, n INTEGER)"));
	sql.Add(wxT("CREATE TABLE f.fd(d INTEGER PRIMARY KEY, n INTEGER)"));
	sql.Add(wxT("CREATE TABLE f.fs(s INTEGER PRIMARY KEY, n INTEGER)"));
	sql.Add(wxT("CREATE TABLE f.fg(g CHAR(2), n INTEGER)"));

	sql.Add(wxT("INSERT INTO tb(b, a, d) SELECT DISTINCT id, id_author, created FROM books WHERE 1") + m_filter.GetFilterSQL());
	sql.Add(wxT("INSERT INTO f.fa(a, n) SELECT a, COUNT(b) FROM tb GROUP BY a"));
	sql.Add(wxT("INSERT INTO f.fd(d, n) SELECT d, COUNT(b) FROM tb GROUP BY d"));
	sql.Add(wxT("INSERT INTO f.fs(s, n) SELECT id_seq, COUNT(DISTINCT b) FROM tb INNER JOIN bookseq ON b=id_book GROUP BY id_seq"));
	sql.Add(wxT("INSERT INTO f.fg(g, n) SELECT id_genre, COUNT(DISTINCT b) FROM tb INNER JOIN genres ON b=id_book GROUP BY id_genre"));

	size_t count = sql.Count();
	for (size_t i = 0; i < count; i++) {
		database.ExecuteUpdate(sql[i]);
		if (IsClosed()) return false;
	}

	trans.Commit();

	return true;
}
