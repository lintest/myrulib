#include <wx/zipstrm.h>
#include "FbThread.h"
#include "FbParams.h"
#include "FbParser.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "Sequences.h"
#include "MyRuLibMain.h"

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;
extern wxString strParsingInfo;

FbThread::FbThread(wxEvtHandler *frame, const wxString &filename, bool update)
        : wxThread(), m_filename(filename), m_frame(frame), m_update(update)
{

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

	if (full_name.IsEmpty()) return 0;

	wxString search_name = full_name;
	MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Name(search_name);

	if (!row) {
		wxString letter = search_name.Left(1);
		MakeUpper(letter);
		if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");
		row = authors.New();
		row->id = (full_name.IsEmpty() ? 0 : NewId(DB_NEW_AUTHOR));
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = full_name;
		row->Save();
	}
	return row->id;
}

int FbThread::FindSequence(wxString &name) {

	if (name.IsEmpty()) return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * seqRow = sequences.Name(name);

	if (!seqRow) {
		seqRow = sequences.New();
		seqRow->id = - NewId(DB_NEW_SEQUENCE);
		seqRow->value = name;
		seqRow->Save();
	}

	return seqRow->id;
}

void FbThread::AddSequence(int id_book, wxString &name, wxString &number) {

	if (name.IsEmpty()) return;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * seqRow = sequences.Name(name);

	if (!seqRow) {
		seqRow = sequences.New();
		seqRow->id = NewId(DB_NEW_SEQUENCE);
		seqRow->value = name;
		seqRow->Save();
	}

	long num = 0;
	name.ToLong(&num);

	Bookseq bookseq(wxGetApp().GetDatabase());
	BookseqRow * bookRow = bookseq.New();
	bookRow->id_book = id_book;
	bookRow->id_seq = seqRow->id;
//	bookRow->id_author = ??;
	bookRow->number = num;
}

bool FbThread::UpdateXml(const wxString &name, int id_archive)
{
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

    wxFileName filename (name);
    wxString shortname = filename.GetName();
    unsigned long number = 0;
    if (!shortname.ToULong(&number)) return false;

	DatabaseLayer * database = wxGetApp().GetDatabase();

	wxString sql = wxString::Format(wxT("UPDATE books SET id_archive=%d WHERE id=%d"), id_archive, number);
	database->RunQuery(sql);
	return true;
}

struct SeqItem {
public:
	SeqItem(int s, int n): seq(s), num(n) {};
	int seq;
	int num;

};

#include <wx/arrimpl.cpp>

WX_DECLARE_OBJARRAY(SeqItem, SeqItemArray);
WX_DEFINE_OBJARRAY(SeqItemArray);

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
	SeqItemArray seqArray;

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
			    wxString name = node->Prop(wxT("name"));
			    int seq = FindSequence(name);
				if (seq) {
					wxString number = node->Prop(wxT("number"));
					long num = 0;
					number.ToLong(&num);
					seqArray.Add(SeqItem(seq, num));
				}
			}
        }
		node = node->m_next;
    }

	if (book_authors.Count() == 0) book_authors.Add(0);

	long id_book = 0;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		wxFileName filename (name);
		wxString shortname = filename.GetName();
		if (shortname.ToLong(&id_book)) {
			wxString query = wxString::Format(wxT("DELETE FROM books WHERE id=%d"), id_book);
			wxGetApp().GetDatabase()->RunQuery(query);
		} else {
			id_book = - NewId(DB_NEW_BOOK);
		}
	}

	for (size_t i = 0; i<book_authors.Count(); i++) {
		wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
		Books books(wxGetApp().GetDatabase());
		Bookseq bookseq(wxGetApp().GetDatabase());

		BooksRow * row = books.New();
		row->id = id_book;
		row->id_author = book_authors[i];
		row->title = book_title;
		row->annotation = annotation;
		row->genres = genres;
		row->file_size = size;
		row->file_name = name;
		row->id_archive = id_archive;
		row->Save();

		for (size_t j = 0; j<seqArray.Count(); j++) {
			BookseqRow * seqRow = bookseq.New();
			seqRow->id_book = id_book;
			seqRow->id_seq = seqArray[j].seq;
			seqRow->id_author = book_authors[i];
			seqRow->number = seqArray[j].num;
			seqRow->Save();
		}
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
				if (m_update)
                    UpdateXml(filename, id_archive);
				else
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

