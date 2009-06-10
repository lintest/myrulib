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
#include "RecordIDClientData.h"

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
	static int NewId(int param);
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
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Archives archives(wxGetApp().GetDatabase());
	ArchivesRow * row = archives.New();
	row->id = NewId(DB_NEW_ARCHIVE);
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
		row->id = NewId(DB_NEW_AUTHOR);
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
	int new_id = NewId(DB_NEW_BOOK);

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
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

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

class SequenceNode {
public:
    SequenceNode(const int id, const wxTreeItemId &item)
            : m_id(id), m_item(item),  m_next(NULL) {};
	virtual ~SequenceNode() {};
public:
    int m_id;
    wxTreeItemId m_item;
    SequenceNode *m_next;
};

class SequenceList {
public:
	SequenceList()
		: m_root(NULL), m_last(NULL) {};
    virtual ~SequenceList();
	void Append(const int id, const wxTreeItemId &item);
	wxTreeItemId Find(const int id, wxTreeItemId root);
public:
    SequenceNode *m_root;
    SequenceNode *m_last;
};

SequenceList::~SequenceList() 
{
	while (m_root) {
		SequenceNode * node = m_root;
		m_root = node->m_next;
		delete node;
	}
}

void SequenceList::Append(const int id, const wxTreeItemId &item)
{
	SequenceNode * node = new SequenceNode(id, item);
	if (m_root)
		m_last->m_next = node;
	else
		m_root = node;
	m_last = node;
}

wxTreeItemId SequenceList::Find(const int id, wxTreeItemId root) 
{
	SequenceNode * node = m_root;
	while (node) {
		if (node->m_id == id)
			return node->m_item;
		node = node->m_next;
	}
	return root;
}

void FbManager::FillBooks(wxTreeListCtrl * treelist, int id_author) {

	treelist->Freeze();

    treelist->DeleteRoot();
    wxTreeItemId root = treelist->AddRoot (_T("Root"));

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * thisAuthor = authors.Id(id_author);
	if(thisAuthor)
	{
		BooksRowSet * allBooks = thisAuthor->GetBooks(wxT("id_sequence, title"));

		int id_sequence = 0;
		wxString sequencesText;
		for(size_t i = 0; i < allBooks->Count(); i++){
		    BooksRow * thisBook = allBooks->Item(i);
			if (id_sequence != thisBook->id_sequence) {
				if (!sequencesText.IsEmpty())
					sequencesText += wxT(",");
				id_sequence = thisBook->id_sequence;
				sequencesText += wxString::Format(wxT("%d"), id_sequence);
			}
		}

		SequenceList sequencesList;

		if (!sequencesText.IsEmpty()) {
			wxString whereCause = wxString::Format(wxT("id in(%s)"), sequencesText);
			Sequences sequences(wxGetApp().GetDatabase());
			SequencesRowSet * allSequences = sequences.WhereSet(whereCause, wxT("value"));

			for(size_t i = 0; i < allSequences->Count(); i++) {
				SequencesRow * thisSequence = allSequences->Item(i);
				wxTreeItemId item = treelist->AppendItem(root, thisSequence->value, 0);
				treelist->SetItemBold(item, true);
				sequencesList.Append(thisSequence->id, item);
			}
		}

		for(size_t i = 0; i < allBooks->Count(); i++) {
		    BooksRow * thisBook = allBooks->Item(i);
			wxTreeItemId parent = root;
			if (thisBook->id_sequence) 
				parent = sequencesList.Find(thisBook->id_sequence, root);
			wxTreeItemId item = treelist->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook->id));
			treelist->SetItemText (item, 1, thisBook->file_name);
			treelist->SetItemText (item, 2, wxString::Format(wxT("%d"), thisBook->file_size));
		}
	}
    treelist->ExpandAll(root);

	treelist->Thaw();
}


void FbManager::FillAuthors(wxListBox *listbox, const wxChar & findLetter)
{
    const wxString orderBy = wxT("search_name");
	wxString findText = findLetter;
    const wxString whereClause = wxString::Format(wxT("letter = '%s'"), findText.c_str());

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRowSet * allAuthors = authors.WhereSet(whereClause, orderBy);
	FillAuthors(listbox, allAuthors);
}

void FbManager::FillAuthors(wxListBox *listbox, const wxString & findText)
{
    const wxString orderBy = wxT("search_name");

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRowSet * allAuthors;

	if (findText.IsEmpty()) {
        allAuthors = authors.All(orderBy);
    } else {
		wxString text = findText;
		FbManager::MakeLower(text);
        const wxString whereClause = wxString::Format(wxT("search_name like '%s%%'"), text.c_str());
        allAuthors = authors.WhereSet(whereClause, orderBy);
    }
	FillAuthors(listbox, allAuthors);
}

void FbManager::FillAuthors(wxListBox *listbox, AuthorsRowSet * allAuthors)
{
	listbox->Freeze();
	listbox->Clear();

	for(unsigned long i = 0; i < allAuthors->Count(); i++) {
		listbox->Append(allAuthors->Item(i)->full_name,
			new RecordIDClientData(allAuthors->Item(i)->id));
	}

	listbox->Thaw();
}

