#include "TitleThread.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "MyRuLibApp.h"
#include "ZipReader.h"
#include "db/Files.h"
#include "BookExtractInfo.h"

void TitleThread::Execute(wxEvtHandler *frame, const int id)
{
    if (!id) return;
	TitleThread *thread = new TitleThread(frame, id);
    if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

void *TitleThread::Entry()
{
    InfoCash::SetTitle(m_id, GetBookInfo(m_id));

    InfoCash::SetFilelist(m_id, GetBookFiles(m_id));

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOKINFO_UPDATE );
    event.SetInt(m_id);
    wxPostEvent( m_frame, event );

	return NULL;
}

//! Return specified string with the html special characters encoded.  Similar to PHP's htmlspecialchars() function.
wxString TitleThread::HTMLSpecialChars( const wxString &value, const bool bSingleQuotes, const bool bDoubleQuotes )
{
  wxString szToReturn = value;
  szToReturn.Replace(wxT("&"),wxT("&amp;"));
  if( bSingleQuotes )
    szToReturn.Replace(wxT("'"),wxT("&#039;"));
  if( bDoubleQuotes )
    szToReturn.Replace(wxT("\""), wxT("&quot;"));
  szToReturn.Replace(wxT("<"),wxT("&lt;"));
  szToReturn.Replace(wxT(">"),wxT("&gt;"));
  return szToReturn;
}

wxString TitleThread::GetBookInfo(int id)
{
    wxString authors, title, annotation, genres;

    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
        DatabaseLayer * database = wxGetApp().GetDatabase();

        wxString sql = wxT("\
            SELECT authors.full_name FROM authors  \
            WHERE id IN (SELECT id_author FROM books WHERE id=?) \
            ORDER BY authors.full_name \
        ");

        PreparedStatement * ps = database->PrepareStatement(sql);
        if (ps) {
            ps->SetParamInt(1, id);
            DatabaseResultSet* result = ps->ExecuteQuery();
            while ( result && result->Next() ) {
                if (!authors.IsEmpty()) authors += wxT(", ");
                authors += result->GetResultString(1);
            }
            database->CloseResultSet(result);
        }
        database->CloseStatement(ps);

        Books books(database);
        BooksRow * thisBook = books.Id(id);
        title = thisBook->title;
        genres = FbGenres::DecodeList(thisBook->genres);
    }

    wxString html;

    html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), HTMLSpecialChars(authors).c_str());

    if (!genres.IsEmpty())
        html += wxString::Format(wxT("<br><font size=3>%s</font>"), HTMLSpecialChars(genres).c_str());

    html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTMLSpecialChars(title).c_str());

    return html;
}

wxString TitleThread::GetBookFiles(int id)
{
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    DatabaseLayer * database = wxGetApp().GetDatabase();

    wxString sql = wxT("\
        SELECT DISTINCT 0 AS Key, id, id_archive, file_name, file_path FROM books WHERE id=? UNION ALL \
        SELECT DISTINCT 1 AS Key, id_book, id_archive, file_name, file_path FROM files WHERE id_book=? \
        ORDER BY Key \
    ");

	BookExtractInfoArray items;

    PreparedStatement * ps = database->PrepareStatement(sql);
    if (!ps) return wxEmptyString;
    ps->SetParamInt(1, id);
    ps->SetParamInt(2, id);

    DatabaseResultSet* result = ps->ExecuteQuery();
    while ( result && result->Next() ) {
        items.Add(result);
    }
    database->CloseResultSet(result);
    database->CloseStatement(ps);

    for (size_t i = 0; i<items.Count(); i++) {
        BookExtractInfo & item = items[i];
        if (!item.id_archive) continue;
        Archives archives(wxGetApp().GetDatabase());
        ArchivesRow * row = archives.Id(item.id_archive);
        if (!row) continue;
        item.zip_name = row->file_name;
        item.zip_path = row->file_path;
        if (item.zip_path.IsEmpty()) item.zip_path = wxT("$(WANRAIK)");
    }

    wxString html;

	for (size_t i = 0; i<items.Count(); i++) {
		BookExtractInfo & item = items[i];
        if ( item.id_book > 0 )
            html += wxString::Format(wxT("<p>$(LIBRUSEC)/%s</p>"), item.GetBook().c_str());
        else if ( item.id_archive )
            html += wxString::Format(wxT("<p>%s: %s</p>"), item.GetZip().c_str(), item.GetBook().c_str());
        else
            html += wxString::Format(wxT("<p>%s</p>"), item.GetBook().c_str());
	}

    return html;
}

