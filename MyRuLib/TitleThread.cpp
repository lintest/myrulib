#include "TitleThread.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "MyRuLibApp.h"

void TitleThread::Execute(wxEvtHandler *frame, const int id)
{
    if (!id) return;
	TitleThread *thread = new TitleThread(frame, id);
    if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

void *TitleThread::Entry()
{
    wxString html = GetBookInfo(m_id);
    InfoCash::SetTitle(m_id, html);

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
    wxString title, annotation;
    wxString authorText, genreText;

    {
        wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

        Books books(wxGetApp().GetDatabase());
        wxString whereClause = wxString::Format(wxT("id=%d"), id);
        BooksRowSet * allBooks = books.WhereSet( whereClause, wxT("title"));
        wxString genres, file_name, file_type;
        wxStringList authorList;
        for(size_t i = 0; i < allBooks->Count(); ++i) {
            BooksRow * thisBook = allBooks->Item(i);
            Authors authors(wxGetApp().GetDatabase());
            title = thisBook->title;
            genres = thisBook->genres;
            file_type = thisBook->file_type;
            file_name = thisBook->file_name;
            authorList.Add(authors.Id(thisBook->id_author)->full_name);
        }

        if (file_type != wxT("fb2")) InfoCash::SetAnnotation(id, file_name);

        authorList.Sort();
        for (size_t i = 0; i<authorList.GetCount(); i++) {
            if (!authorText.IsEmpty()) authorText += wxT(", ");
            authorText += authorList[i];
        }
        for (size_t i = 0; i<genres.Len()/2; i++) {
            if (!genreText.IsEmpty()) genreText += wxT(", ");
            wxString genreCode = genres.SubString(i*2, i*2+1);
            genreText +=  FbGenres::Name( genreCode );
        }
    }

    wxString html;

    html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), HTMLSpecialChars(authorText).c_str());

    if (!genreText.IsEmpty())
        html += wxString::Format(wxT("<br><font size=3>%s</font>"), HTMLSpecialChars(genreText).c_str());

    html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), HTMLSpecialChars(title).c_str());

    return html;
}

