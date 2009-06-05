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
#include "MyRuLibApp.h"
#include "Authors.h"
#include "Books.h"
#include "Params.h"

enum {
	DB_LIBRARY_TITLE = 1,
	DB_NEW_ARCHIVE,
	DB_NEW_AUTHOR,
	DB_NEW_BOOK,
};

void FbManager::InitParams(DatabaseLayer *database) 
{
	database->RunQuery(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
	database->RunQuery(_("INSERT INTO params(text) VALUES ('Test Library');"));
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
private:
	bool ParseXml(wxInputStream& stream, const wxString &name, const wxFileOffset size, int id_archive);
    int AddArchive();
	int FindAuthor(wxString &full_name);
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

	wxString letter = full_name.Left(1);
	FbManager::MakeUpper(letter);

	if (alphabet.Find(letter) == wxNOT_FOUND)
		letter = wxT("#");

	const wxString& whereClause = wxString::Format(_("search_name='%s'"), search_name.c_str());

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Where(whereClause);

	if (!row) {
		row = authors.New();
		row->id = FbManager::NewId(DB_NEW_AUTHOR);
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = full_name;
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
	wxArrayString book_genres;
	wxString book_title;

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        wxString value;
        if ( name == wxT("author") ) {
            value = xml.GetAuthor(node);
			book_authors.Add( FindAuthor(value) );
		} else {
			value = node->m_text;
			if ( name == wxT("genre") ) {
				book_genres.Add(value);
			} else if ( name == wxT("book-title") ) {
				book_title = value;
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
		row->title = book_title;
		row->file_size = size /1024;
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

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_THREAD_EVENT );
	event.SetExtraLong(zip.GetTotalEntries());
    wxPostEvent( m_frame, event );

	int progress = 0;
	while (entry = zip.GetNextEntry()) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_THREAD_EVENT );
		event.SetString(entry->GetName());
		event.SetInt(progress++);
        wxPostEvent( m_frame, event );

		zip.OpenEntry(*entry);
		ParseXml(zip, entry->GetName(), entry->GetSize(), id_archive);
		delete entry;
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

int FbManager::FindAuthor(wxString &full_name) {

	wxString search_name = full_name;
	MakeLower(search_name);

	wxString letter = full_name.Left(1);
	MakeUpper(letter);

	if (alphabet.Find(letter) == wxNOT_FOUND)
		letter = wxT("#");

	const wxString& whereClause = wxString::Format(_("search_name='%s'"), search_name.c_str());

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Where(whereClause);

	if (!row) {
		row = authors.New();
		row->id = NewId(DB_NEW_AUTHOR);
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = full_name;
		row->Save();
	}
	return row->id;
}

bool FbManager::ParseXml(const wxString& filename, wxString& html) 
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return false;

	wxFile file(filename);
	wxFileOffset size = file.Length() / 1024;

    return ParseXml(stream, html, filename, size);
}

bool FbManager::ParseZip(const wxString& filename, wxString& html) 
{
	FbThread *thread = new FbThread(wxGetApp().GetTopWindow(), filename);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

	/*
    wxProgressDialog * m_dlgProgress = new wxProgressDialog
                        (
                         _T("Progress dialog"),
                         _T("Wait until the thread terminates or press [Cancel]"),
                         100,
						 wxGetApp().GetTopWindow(),
                         wxPD_CAN_ABORT |
                         wxPD_APP_MODAL |
                         wxPD_ELAPSED_TIME |
                         wxPD_ESTIMATED_TIME |
                         wxPD_REMAINING_TIME
                        );
	*/

    return true;
}

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
	wxArrayString book_genres;
	wxString book_title;

#ifdef FB_DEBUG_PARSING
	xml.GetRoot()->Print(html);
#endif //FB_DEBUG_PARSING

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        wxString value;
        if ( name == wxT("author") ) {
            value = xml.GetAuthor(node);
			book_authors.Add( FindAuthor(value) );
		} else {
			value = node->m_text;
			if ( name == wxT("genre") ) {
				book_genres.Add(value);
			} else if ( name == wxT("book-title") ) {
				book_title = value;
			}
        }
        html += wxString::Format(wxT("<b>%s:</b>&nbsp;%s<br>"), name.c_str(), value.c_str());
		node = node->m_next;
    }

	Books books(wxGetApp().GetDatabase());
	int new_id = NewId(DB_NEW_BOOK);

	size_t iConut = book_authors.Count();
	for (size_t i = 0; i<iConut; i++) {
		BooksRow * row = books.New();
		row->id = new_id;
		row->id_author = book_authors[i];
		row->title = book_title;
		row->file_size = size /1024;
		row->file_name = name;
		row->id_archive = id_archive;
		row->Save();
	}

    html += wxT("<hr>");

	return true;

}
