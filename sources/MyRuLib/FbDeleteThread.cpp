#include "FbDeleteThread.h"
#include "FbExtractInfo.h"
#include "FbParams.h"
#include "MyRuLibApp.h"

void * FbDeleteThread::Entry()
{
	FbCommonDatabase database;

	if (FbParams(FB_REMOVE_FILES)) DoDelete(database);

	wxString sql;

	const wxChar * tables[][2] = {
		{ wxT("books"),    wxT("id") },
		{ wxT("bookseq"),  wxT("id_book") },
		{ wxT("files"),    wxT("id_book") },
		{ wxT("genres"),   wxT("id_book") },
		{ wxT("fts_book"), wxT("docid") },
	};

	size_t count = sizeof(tables) / sizeof(wxChar*) / 2;
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxString::Format(wxT("DELETE FROM %s WHERE %s IN (%s)"), tables[i][0], tables[i][1], m_sel.c_str());
		database.ExecuteUpdate(sql);
	}

	return NULL;
}

void FbDeleteThread::DoDelete(FbDatabase &database)
{
	wxString basepath = wxGetApp().GetLibPath();
	wxString sql = wxString::Format(wxT("SELECT id FROM books WHERE books.id IN (%s)"), m_sel.c_str());
	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		FbExtractArray(database, result.GetInt(0)).DeleteFiles(basepath);
	}
}

/*
void * FbDeleteThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	FbCounter counter(database);
	counter.Add(m_sel);

	if (FbParams(FB_REMOVE_FILES)) DoDelete(database);

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
	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		FbExtractArray(database, result.GetInt(0)).DeleteFiles(basepath);
	}
}
*/
