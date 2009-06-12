#include <wx/zipstrm.h>
#include "FbThread.h"
#include "FbParams.h"
#include "FbParser.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "Sequences.h"
#include "MyRuLibMain.h"

extern wxString strAlphabet;
extern wxString strNobody;
extern wxString strRusJO;
extern wxString strRusJE;
extern wxString strParsingInfo;

FbThread::FbThread(wxEvtHandler *frame, const wxString &filename)
        : wxThread()
{
    m_filename = filename;
    m_frame = frame;
}

void FbThread::MakeLower(wxString & data){
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

void FbThread::MakeUpper(wxString & data){
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

void FbThread::OnExit()
{
}

int FbThread::NewId(int param)
{
	Params params(wxGetApp().GetDatabase());
	ParamsRow * row = params.Id(param);
	row->value++;
	row->Save();

	return row->value;
}

int FbThread::AddArchive()
{
    wxFileName file_name(m_filename);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Archives archives(wxGetApp().GetDatabase());
	ArchivesRow * row = archives.New();
	row->id = NewId(DB_NEW_ARCHIVE);
	row->file_name = file_name.GetFullName();
	row->file_path = file_name.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	row->Save();
	return row->id;
}

int FbThread::FindAuthor(wxString &full_name) {

	wxString search_name = full_name;
	MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Name(search_name);

	if (!row) {
		wxString letter = search_name.Left(1);
		MakeUpper(letter);
		if (letter.IsEmpty()||(strAlphabet.Find(letter) == wxNOT_FOUND))
			letter = wxT("#");
		row = authors.New();
		row->id = NewId(DB_NEW_AUTHOR);
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = (full_name.IsEmpty() ? strNobody : full_name);
		row->Save();
	}
	return row->id;
}

int FbThread::FindSequence(wxString &name) {

	if (name.IsEmpty())
		return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * row = sequences.Name(name);

	if (!row) {
		row = sequences.New();
		row->id = NewId(DB_NEW_SEQUENCE);
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
			if (!value.IsEmpty())
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
			    value = node->Prop(wxT("name"));
				sequence = FindSequence(value);
			}
        }
		node = node->m_next;
    }

	if (book_authors.Count() == 0) {
	    wxString empty = wxEmptyString;
		book_authors.Add(FindAuthor(empty));
	}

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

    wxFileName filename (name);
    wxString shortname = filename.GetName();
    unsigned long number = 0;
    long id = 0;
    if (shortname.ToULong(&number)) {
        wxString query = wxString::Format(wxT("DELETE FROM books WHERE id=%d"), number);
        wxGetApp().GetDatabase()->RunQuery(query);
        id = number;
    } else {
        id = - NewId(DB_NEW_BOOK);
    }

	Books books(wxGetApp().GetDatabase());

	size_t iConut = book_authors.Count();
	for (size_t i = 0; i<iConut; i++) {
		BooksRow * row = books.New();
		row->id = id;
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
    wxCriticalSectionLocker enter(wxGetApp().m_ThreadQueue);

	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	int id_archive = AddArchive();

	{
		wxFileName filename = m_filename;
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_START );
		event.SetInt(zip.GetTotalEntries());
		event.SetString(strParsingInfo + filename.GetFullName());
		wxPostEvent( m_frame, event );
	}

	int progress = 0;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		if (entry->GetSize()) {
			wxString filename = entry->GetName(wxPATH_UNIX);
			if (filename.Right(4).Lower() == wxT(".fb2")) {
				wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_UPDATE );
				event.SetString(filename);
				event.SetInt(progress++);
				wxPostEvent( m_frame, event );

				zip.OpenEntry(*entry);
				ParseXml(zip, filename, entry->GetSize(), id_archive);
			}
		}
		delete entry;
	}

	{
		wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyRuLibMainFrame::ID_PROGRESS_FINISH );
		wxPostEvent( m_frame, event );
	}

	return NULL;
}

