#include "TitleThread.h"
#include "FbGenres.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "ZipReader.h"
#include "BookExtractInfo.h"

wxCriticalSection TitleThread::sm_queue;

void *TitleThread::Entry()
{
	wxCriticalSectionLocker enter(sm_queue);

	FbCommonDatabase database;

	InfoCash::SetTitle(m_id, GetBookInfo(database, m_id));
	InfoCash::SetFilelist(m_id, GetBookFiles(database, m_id));

	UpdateInfo();

	return NULL;
}

wxString TitleThread::GetBookInfo(FbDatabase &database, int id)
{
	wxString authors, title, annotation, genres;

	{
		wxString sql = wxT("\
			SELECT authors.full_name FROM authors  \
			WHERE id IN (SELECT id_author FROM books WHERE id=?) \
			ORDER BY authors.full_name \
		");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		while ( result.NextRow() ) {
			if (!authors.IsEmpty()) authors += wxT(", ");
			authors += result.GetString(0).Trim(true).Trim(false);
		}
	}

	{
		wxString sql = wxT("SELECT title, genres FROM books WHERE id=? LIMIT 1");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) {
			title = result.GetString(wxT("title"));
			genres = result.GetString(wxT("genres"));
		}
	}
	genres = FbGenres::DecodeList(genres);

	wxString html;

	html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), HTMLSpecialChars(authors).c_str());

	if (!genres.IsEmpty())
		html += wxString::Format(wxT("<br><font size=3>%s</font>"), HTMLSpecialChars(genres).c_str());

	html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTMLSpecialChars(title).c_str());

	return html;
}

wxString TitleThread::GetBookFiles(FbDatabase &database, int id)
{
	BookExtractArray items(database, id);

	wxString html;

	for (size_t i = 0; i<items.Count(); i++) {
		BookExtractInfo & item = items[i];
		if (item.librusec) {
			html += wxString::Format(wxT("<p>$(LIBRUSEC)/%s</p>"), item.book_name.c_str());
		} else if ( item.id_archive ) {
			if (item.NameIsEqual())
				html += wxString::Format(wxT("<p>%s</p>"), item.zip_name.c_str());
			else
				html += wxString::Format(wxT("<p>%s: %s</p>"), item.zip_name.c_str(), item.book_name.c_str());
		} else {
			html += wxString::Format(wxT("<p>%s</p>"), item.book_name.c_str());
		}
	}

	return html;
}

