#include "TitleThread.h"
#include "FbGenres.h"
#include "InfoCash.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "FbExtractInfo.h"

wxCriticalSection TitleThread::sm_queue;

void *TitleThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	FbCommonDatabase database;

	GetBookInfo(database);
	GetBookFiles(database);

	UpdateInfo();

	return NULL;
}

wxString TitleThread::GetBookInfo(FbDatabase &database)
{
	wxString authors, title, description, genres;

	{
		wxString sql = wxT("\
			SELECT authors.full_name FROM authors  \
			WHERE id IN (SELECT id_author FROM books WHERE id=?) \
			ORDER BY authors.full_name \
		");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		while ( result.NextRow() ) {
			if (!authors.IsEmpty()) authors += wxT(", ");
			authors += result.GetString(0).Trim(true).Trim(false);
		}
	}

	{
		wxString sql = wxT("SELECT title, genres, description FROM books WHERE id=? LIMIT 1");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) {
			title = result.GetString(0);
			genres = result.GetString(1);
			description = result.GetString(2);
		}
	}
	genres = FbGenres::DecodeList(genres);

	wxString html;

	html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), HTMLSpecialChars(authors).c_str());

	if (!genres.IsEmpty())
		html += wxString::Format(wxT("<br><font size=3>%s</font>"), HTMLSpecialChars(genres).c_str());

	html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTMLSpecialChars(title).c_str());

	InfoCash::SetTitle(m_id, html, description);

	return html;
}

wxString TitleThread::GetBookFiles(FbDatabase &database)
{
	FbExtractArray items(database, m_id);

	wxString html;

	for (size_t i = 0; i<items.Count(); i++) {
		html += wxString::Format(wxT("<p>%s</p>"), items[i].NameInfo().c_str());
	}

	InfoCash::SetFilelist(m_id, html);

	return html;
}

