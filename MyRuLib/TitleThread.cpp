#include "TitleThread.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "MyRuLibApp.h"
#include "ZipReader.h"
#include "db/Files.h"

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

    Books books(database);
    BooksRow * bookRow = books.Id(id);
    if (!bookRow) return wxEmptyString;

    wxString html;

    if ( bookRow->id>0 ) {
        html += wxString::Format(wxT("<p>$(LIBRUSEC)/%s</p>"), bookRow->file_name.c_str());
    } else {
        html += wxString::Format(wxT("<p>%s%s</p>"), GetArchivePath(bookRow->id_archive).c_str(), bookRow->file_name.c_str());
    }

    Files files(database);
    FilesRowSet * fileRows = files.IdBook(bookRow->id);
    for (size_t i = 0; i<fileRows->Count(); i++) {
        FilesRow * row = fileRows->Item(i);
        html += wxString::Format(wxT("<p>%s%s</p>"), GetArchivePath(row->id_archive).c_str(), row->file_name.c_str());
    }
    return html;
}

wxString TitleThread::GetArchivePath(int id_archive)
{
    if ( ! id_archive ) return wxEmptyString;

    DatabaseLayer * database = wxGetApp().GetDatabase();
    Archives archives(database);

    ArchivesRow * archiveRow = archives.Id(id_archive);
    if (archiveRow) {
        if (archiveRow->file_path.IsEmpty()) {
            return wxString::Format(wxT("$(WANRAIK)/%s: "),  archiveRow->file_name.c_str());
        } else {
            wxFileName zipname = archiveRow->file_name;
            zipname.SetPath(archiveRow->file_path);
            return zipname.GetFullPath() + wxT(": ");
        }
    }
    return wxEmptyString;
}
