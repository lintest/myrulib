/***************************************************************
 * Name:      FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/progdlg.h>
#include "FbThread.h"
#include "FbParams.h"
#include "FbManager.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "Sequences.h"
#include "Bookseq.h"

bool FbManager::ParseXml(const wxString& filename, wxString& html)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok()) return false;

	wxFile file(filename);
	wxFileOffset size = file.Length() / 1024;

	return FbThread::ParseXml(stream, filename, size, 0);
}

bool FbManager::RegisterZip(const wxString& filename)
{
	FbThread *thread = new FbThread(wxGetApp().GetTopWindow(), filename, true);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
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
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

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

extern wxString strOtherSequence;

void FbManager::FillBooks(wxTreeListCtrl * treelist, int id_author) {

	treelist->Freeze();

    treelist->DeleteRoot();
    wxTreeItemId root = treelist->AddRoot (_T("Root"));

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * thisAuthor = authors.Id(id_author);
	if(thisAuthor)
	{
		BookseqRowSet * bookseq = thisAuthor->GetBookseqs();

		int id_seq = 0;
		wxString sequencesText;
		for(size_t i = 0; i < bookseq->Count(); i++){
		    BookseqRow * seqRow = bookseq->Item(i);
			if (id_seq == seqRow->id_seq) continue;

            if (!sequencesText.IsEmpty()) sequencesText += wxT(",");
            id_seq = seqRow->id_seq;
            sequencesText += wxString::Format(wxT("%d"), id_seq);
		}

		BooksRowSet * allBooks = thisAuthor->GetBooks(wxT("title"));
		for(size_t i = 0; i < allBooks->Count(); i++) {
            allBooks->Item(i)->added = false;
		}

		if (!sequencesText.IsEmpty()) {
			wxString whereCause = wxString::Format(wxT("id in(%s)"), sequencesText.c_str());
			Sequences sequences(wxGetApp().GetDatabase());
			SequencesRowSet * allSequences = sequences.WhereSet(whereCause, wxT("value"));

			for(size_t i = 0; i < allSequences->Count(); i++) {
				SequencesRow * thisSequence = allSequences->Item(i);
				wxTreeItemId parent = treelist->AppendItem(root, thisSequence->value, 0);
				treelist->SetItemBold(parent, true);

                for (size_t j = 0; j < bookseq->Count(); j++) {
                    BookseqRow * seqRow = bookseq->Item(j);
                    if (seqRow->id_seq != thisSequence->id) continue;

                    for(size_t k = 0; k < allBooks->Count(); k++) {
                        BooksRow * thisBook = allBooks->Item(k);
                        if (seqRow->id_book == thisBook->id) seqRow->order = k;
                    }
                }
                bookseq->SortBy(wxT("number,order"));

                for (size_t j = 0; j < bookseq->Count(); j++) {
                    BookseqRow * seqRow = bookseq->Item(j);
                    if (seqRow->id_seq != thisSequence->id) continue;

                    for(size_t k = 0; k < allBooks->Count(); k++) {
                        BooksRow * thisBook = allBooks->Item(k);
                        if (seqRow->id_book != thisBook->id) continue;

                        wxTreeItemId item = treelist->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook->id));
                        if (seqRow->number>0) treelist->SetItemText (item, 1, wxString::Format(wxT("%d"), seqRow->number));
                        treelist->SetItemText (item, 2, thisBook->file_name);
                        treelist->SetItemText (item, 3, wxString::Format(wxT("%d"), thisBook->file_size/1024));
                        thisBook->added = true;
                    }
                }
			}
		}

		wxTreeItemId parent = root;
		for(size_t i = 0; i < allBooks->Count(); i++) {
		    BooksRow * thisBook = allBooks->Item(i);
		    if (thisBook->added) continue;
		    if (parent == root) {
                parent = treelist->AppendItem(root, strOtherSequence, 0);
                treelist->SetItemBold(parent, true);
		    }
            wxTreeItemId item = treelist->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook->id));
            treelist->SetItemText (item, 2, thisBook->file_name);
            treelist->SetItemText (item, 3, wxString::Format(wxT("%d"), thisBook->file_size/1024));
		}
	}
    treelist->ExpandAll(root);

	treelist->Thaw();
}


void FbManager::FillAuthorsChar(wxListBox *listbox, const wxChar & findLetter)
{
    const wxString orderBy = wxT("search_name");
	wxString findText = findLetter;
    const wxString whereClause = wxString::Format(wxT("letter = '%s'"), findText.c_str());

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT id, full_name, first_name, middle_name, last_name FROM authors WHERE letter=? ORDER BY search_name");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, findLetter);
	DatabaseResultSet* result = pStatement->ExecuteQuery();
	FillAuthors(listbox, result);
	wxGetApp().GetDatabase()->CloseStatement(pStatement);
}

void FbManager::FillAuthorsText(wxListBox *listbox, const wxString & findText)
{
    const wxString orderBy = wxT("search_name");

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT id, full_name, first_name, middle_name, last_name FROM authors WHERE search_name like ? ORDER BY search_name");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, findText + wxT("%"));
	DatabaseResultSet* result = pStatement->ExecuteQuery();
	FillAuthors(listbox, result);
	wxGetApp().GetDatabase()->CloseStatement(pStatement);
}

void FbManager::FillAuthors(wxListBox *listbox, DatabaseResultSet* result)
{
	listbox->Freeze();
	listbox->Clear();

	if(result){
		while(result->Next()){
			int id = result->GetResultInt(wxT("id"));
			wxString full_name = result->GetResultString(wxT("full_name"));
			wxString first_name  = result->GetResultString(wxT("first_name"));
			wxString middle_name = result->GetResultString(wxT("middle_name"));
			wxString last_name = result->GetResultString(wxT("last_name"));
			listbox->Append(full_name, new RecordIDClientData(id));
		}
	}

	listbox->Thaw();
}

class TempFileEraser {
private:
    wxStringList filelist;
    TempFileEraser() {};
    virtual ~TempFileEraser();
public:
    static void Add(const wxString &filename);
};

TempFileEraser::~TempFileEraser()
{
    for (size_t i=0; i<filelist.GetCount(); i++)
        wxRemoveFile(filelist[i]);
};

void TempFileEraser::Add(const wxString &filename)
{
    static TempFileEraser eraser;
    eraser.filelist.Add(filename);
};

void FbManager::OpenBook(int id)
{
    wxString fbreader = FbParams().GetText(FB_FB2_PROGRAM);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

    Books books(wxGetApp().GetDatabase());
    BooksRow * bookRow = books.Id(id);

    if (!bookRow) return;

    if (bookRow->id_archive)
	{
        ArchivesRow * archiveRow = bookRow->GetArchive();
        wxFileName zip_name = archiveRow->file_path + archiveRow->file_name;
        if (!zip_name.FileExists()) {
            zip_name = wxGetApp().GetAppPath() + archiveRow->file_name;
            if (!zip_name.FileExists()) return;
        }

        wxFFileInputStream in(zip_name.GetFullPath());
        wxZipInputStream zip(in);

		bool find_ok = false;
		bool open_ok = false;
		while (wxZipEntry * entry = zip.GetNextEntry()) {
		    find_ok = (entry->GetName() == bookRow->file_name);
			if (find_ok)
		        open_ok = zip.OpenEntry(*entry);
			delete entry;
			if (find_ok) break;
		}
		if (!find_ok || !open_ok) return;

        wxFileName file_name = wxFileName::CreateTempFileName(wxT("~"));
        wxRemoveFile(file_name.GetFullPath());
        file_name.SetExt(wxT("fb2"));

        wxString file_path = file_name.GetFullPath();
        TempFileEraser::Add(file_path);
        wxFileOutputStream out(file_path);
        out.Write(zip);

        #if defined(__WXMSW__)
		ShellExecute(NULL, NULL, fbreader, file_path, NULL, SW_SHOW);
        #else
        wxExecute(fbreader + wxT(" ") + file_path);
        #endif
    }
	else
	{
//        wxFileName file;
    }

}
