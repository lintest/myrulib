#include "TitleThread.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "ZipReader.h"
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
        wxString sql = wxT("\
            SELECT authors.full_name FROM authors  \
            WHERE id IN (SELECT id_author FROM books WHERE id=?) \
            ORDER BY authors.full_name \
        ");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, id);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        while ( result.NextRow() ) {
            if (!authors.IsEmpty()) authors += wxT(", ");
            authors += result.GetString(0);
        }
    }

    {
        wxString sql = wxT("SELECT title, genres FROM books WHERE id=? LIMIT 1");
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
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

wxString TitleThread::GetBookFiles(int id)
{
	BookExtractArray items(m_database, id);

    wxString html;

	for (size_t i = 0; i<items.Count(); i++) {
		BookExtractInfo & item = items[i];
		if (item.librusec) {
            html += wxString::Format(wxT("<p>$(LIBRUSEC)/%s</p>"), item.GetBook().c_str());
        } else if ( item.id_archive ) {
            if (item.zip_path.IsEmpty()) item.zip_path = wxT("$(WANRAIK)");
            if (item.NameIsEqual())
                html += wxString::Format(wxT("<p>%s</p>"), item.GetZip().c_str());
            else
                html += wxString::Format(wxT("<p>%s: %s</p>"), item.GetZip().c_str(), item.GetBook().c_str());
        } else {
            html += wxString::Format(wxT("<p>%s</p>"), item.GetBook().c_str());
        }
	}

    return html;
}

