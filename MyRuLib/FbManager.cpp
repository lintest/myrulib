/***************************************************************
 * Name:      FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/zipstrm.h>
#include <wx/progdlg.h>
#include "FbManager.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "MyRuLibMain.h"
#include "Authors.h"
#include "Books.h"
#include "Params.h"
#include "Sequences.h"

enum {
	DB_LIBRARY_TITLE = 1,
	DB_LIBRARY_VERSION,
	DB_NEW_ARCHIVE,
	DB_NEW_AUTHOR,
	DB_NEW_BOOK,
	DB_NEW_SEQUENCE,
};

void FbManager::InitParams(DatabaseLayer *database)
{
	database->RunQuery(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
	database->RunQuery(_("INSERT INTO params(text) VALUES ('Test Library');"));
	database->RunQuery(_("INSERT INTO params(value) VALUES (1);"));
	database->RunQuery(_("INSERT INTO params(value) VALUES (1);"));
	database->RunQuery(_("INSERT INTO params(value) VALUES (1);"));
	database->RunQuery(_("INSERT INTO params(value) VALUES (1);"));
	database->RunQuery(_("INSERT INTO params(value) VALUES (1);"));
}

extern wxString alphabet;

class FbThread : public wxThread
{
public:
    FbThread(wxEvtHandler *frame, const wxString &filename);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();
	static int FindAuthor(wxString &full_name);
	static int FindSequence(wxString &name);
	static bool ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive);
    int AddArchive();
private:
    unsigned m_count;
    wxString m_filename;
    wxEvtHandler *m_frame;
};

FbThread::FbThread(wxEvtHandler *frame, const wxString &filename)
        : wxThread()
{
    m_filename = filename;
    m_frame = frame;
}

void FbThread::OnExit()
{
}

int FbThread::AddArchive()
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	Archives archives(wxGetApp().GetDatabase());
	ArchivesRow * row = archives.New();
	row->id = FbManager::NewId(DB_NEW_ARCHIVE);
	row->file_name = m_filename;
	row->Save();
	return row->id;
}

int FbThread::FindAuthor(wxString &full_name) {

	wxString search_name = full_name;
	FbManager::MakeLower(search_name);

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Name(search_name);

	if (!row) {
		wxString letter = full_name.Left(1);
		FbManager::MakeUpper(letter);
		if (letter.IsEmpty()||(alphabet.Find(letter) == wxNOT_FOUND))
			letter = wxT("#");
		row = authors.New();
		row->id = FbManager::NewId(DB_NEW_AUTHOR);
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = (full_name.IsEmpty() ? _("(без автора)") : full_name);
		row->Save();
	}
	return row->id;
}

int FbThread::FindSequence(wxString &name) {

	if (name.IsEmpty())
		return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * row = sequences.Name(name);

	if (!row) {
		row = sequences.New();
		row->id = FbManager::NewId(DB_NEW_SEQUENCE);
		row->value = name;
		row->Save();
	}
	return row->id;
}

bool FbThread::ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive)
{
    FbDocument xml;
	if (!xml.Load(stream, wxT("UTF-8")))
		return false;

	FbNode * node = xml.GetRoot();
	if (!node) return false;

	node = node->Find(wxT("description"));
	if (!node) return false;

	node = node->Find(wxT("title-info"));
	if (!node) return false;

	wxArrayInt book_authors;
	wxString book_title, annotation, genres;
	int sequence = 0;

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        wxString value;
        if ( name == wxT("author") ) {
            value = xml.GetAuthor(node);
			book_authors.Add( FindAuthor(value) );
		} else {
			value = (node->m_text);
			if ( name == wxT("genre") ) {
				genres += FbGenres::Char(value);
			} else if ( name == wxT("book-title") ) {
				book_title = value;
			} else if ( name == wxT("annotation") ) {
				annotation = value;
			} else if ( name == wxT("sequence") ) {
				sequence = FindSequence(node->Prop(wxT("name")));
			}
        }
		node = node->m_next;
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	Books books(wxGetApp().GetDatabase());
	int new_id = FbManager::NewId(DB_NEW_BOOK);

	size_t iConut = book_authors.Count();
	for (size_t i = 0; i<iConut; i++) {
		BooksRow * row = books.New();
		row->id = new_id;
		row->id_author = book_authors[i];
		row->id_sequence = sequence;
		row->title = book_title;
		row->annotation = annotation;
		row->genres = genres;
		row->file_size = size / 1024;
		row->file_name = name;
		row->id_archive = id_archive;
		row->Save();
	}

	return true;
}

void *FbThread::Entry()
{
	wxZipEntry* entry;
	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive();

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(m_filename);
		wxPostEvent( m_frame, event );
	}

	int progress = 0;
	entry = zip.GetNextEntry();
	while (entry) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_UPDATE );
		event.SetString(entry->GetName());
		event.SetInt(progress++);
        wxPostEvent( m_frame, event );

		zip.OpenEntry(*entry);
		ParseXml(zip, entry->GetName(), entry->GetSize(), id_archive);
		delete entry;
        entry = zip.GetNextEntry();
	}

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_FINISH );
		wxPostEvent( m_frame, event );
	}

	return NULL;
}

void FbManager::MakeLower(wxString & data){
#ifdef __WIN32__
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharLower(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeLower();
#endif
}

void FbManager::MakeUpper(wxString & data){
#ifdef __WIN32__
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharUpper(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeUpper();
#endif
}

int FbManager::NewId(int param)
{
	Params params(wxGetApp().GetDatabase());
	ParamsRow * row = params.Id(param);
	row->value++;
	row->Save();

	return row->value;
}

bool FbManager::ParseXml(const wxString& filename, wxString& html)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return false;

	wxFile file(filename);
	wxFileOffset size = file.Length() / 1024;

	return FbThread::ParseXml(stream, filename, size, 0);
}

bool FbManager::ParseZip(const wxString& filename, wxString& html)
{
	FbThread *thread = new FbThread(wxGetApp().GetTopWindow(), filename);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
}
/*
bool FbManager::ParseXml(wxInputStream& stream, wxString& html, const wxString &name, const wxFileOffset size, int id_archive)
{
    FbDocument xml;
	if (!xml.Load(stream, wxT("UTF-8")))
		return false;

	FbNode * node = xml.GetRoot();
	if (!node) return false;

	node = node->Find(wxT("description"));
	if (!node) return false;

	node = node->Find(wxT("title-info"));
	if (!node) return false;

	wxArrayInt book_authors;
	wxString book_title, annotation, genres;

*ifdef FB_DEBUG_PARSING
	xml.GetRoot()->Print(html);
*endif //FB_DEBUG_PARSING

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        wxString value;
        if ( name == wxT("author") ) {
            value = xml.GetAuthor(node);
			book_authors.Add( FbThread::FindAuthor(value) );
		} else {
			value = (node->m_text);
			if ( name == wxT("genre") ) {
				genres += FbGenres::Char(value);
			} else if ( name == wxT("book-title") ) {
				book_title = value;
			} else if ( name == wxT("annotation") ) {
				annotation = value;
			}
        }
		node = node->m_next;
*ifdef FB_DEBUG_PARSING
        html += wxString::Format(wxT("<b>%s:</b>&nbsp;%s<br>"), name.c_str(), value.c_str());
*endif //FB_DEBUG_PARSING
    }

	Books books(wxGetApp().GetDatabase());
	int new_id = NewId(DB_NEW_BOOK);

	size_t iConut = book_authors.Count();
	for (size_t i = 0; i<iConut; i++) {
		BooksRow * row = books.New();
		row->id = new_id;
		row->id_author = book_authors[i];
		row->title = book_title;
		row->annotation = annotation;
		row->genres = genres;
		row->file_size = size /1024;
		row->file_name = name;
		row->id_archive = id_archive;
		row->Save();
		annotation.Empty();
	}

*ifdef FB_DEBUG_PARSING
    html += wxT("<hr>");
*endif //FB_DEBUG_PARSING

	return true;
}
*/
wxString FbManager::BookInfo(int id)
{
    Books books(wxGetApp().GetDatabase());
    wxString whereClause = wxString::Format(wxT("id=%d"), id);
    BooksRowSet * allBooks = books.WhereSet( whereClause, wxT("title"));
    wxString title, annotation, genres;
    wxStringList authorList;
    for(size_t i = 0; i < allBooks->Count(); ++i) {
        BooksRow * thisBook = allBooks->Item(i);
        Authors authors(wxGetApp().GetDatabase());
        title = thisBook->title;
        genres = thisBook->genres;
        if (!thisBook->annotation.IsEmpty()) {
            annotation = thisBook->annotation;
        }
        authorList.Add(authors.Id(thisBook->id_author)->full_name);
    }
    authorList.Sort();
    wxString authorText, genreText;
    for (size_t i = 0; i<authorList.GetCount(); i++) {
        if (!authorText.IsEmpty())
            authorText += wxT(", ");
        authorText += authorList[i];
    }
    for (size_t i = 0; i<genres.Len(); i++) {
        if (!genreText.IsEmpty())
            genreText += wxT(", ");
        genreText +=  FbGenres::Name(genres[i]);
    }

    wxString html(wxT("<html><body>"));

    html += wxString::Format(wxT("<font size=4><b>%s</b></font>"), authorText.c_str());

    if (!genreText.IsEmpty())
        html += wxString::Format(wxT("<br><font size=3>%s</font>"), genreText.c_str());

    html += wxString::Format(wxT("<br><font size=5><b>%s</b></font>"), title.c_str());

    html += annotation;

    html += wxT("</body></html>");

    return html;

}
