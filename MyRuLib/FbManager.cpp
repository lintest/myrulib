/***************************************************************
 * Name:      FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/
#include "FbManager.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/progdlg.h>
#include "RegThread.h"
#include "ImpThread.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "RecordIDClientData.h"
#include "db/Sequences.h"
#include "db/Bookseq.h"
#include "ZipReader.h"

bool FbManager::ParseXml(const wxString& filename)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok()) return false;

	wxFile file(filename);
	wxFileOffset size = file.Length() / 1024;

	return ImportThread::ParseXml(stream, filename, size, 0);
}

bool FbManager::RegisterZip(const wxString& filename)
{
	RegThread *thread = new RegThread(wxGetApp().GetTopWindow(), filename);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
}


bool FbManager::RegisterPath(const wxString& filename)
{
	FolderThread *thread = new FolderThread(wxGetApp().GetTopWindow(), filename);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
}

bool FbManager::ParseZip(const wxString& filename)
{
	ImportThread *thread = new ImportThread(wxGetApp().GetTopWindow(), filename);

    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return false;
    }

    thread->Run();

    return true;
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

void FbManager::FillBooks(wxTreeListCtrl * treelist, int id_author, bool fb2only)
{

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
                        if (fb2only && thisBook->file_type != wxT("fb2")) continue;

                        wxTreeItemId item = treelist->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook, thisSequence->value, seqRow->number));
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
            if (fb2only && thisBook->file_type != wxT("fb2")) continue;
		    if (parent == root) {
                parent = treelist->AppendItem(root, strOtherSequence, 0);
                treelist->SetItemBold(parent, true);
		    }
            wxTreeItemId item = treelist->AppendItem(parent, thisBook->title, 0, -1, new BookTreeItemData(thisBook));
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
    wxString text = findText;
    BookInfo::MakeLower(text);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT id, full_name, first_name, middle_name, last_name FROM authors WHERE search_name like ? ORDER BY search_name");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, text + wxT("%"));
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
    ZipReader reader(id);
    if (!reader.IsOK()) {
        reader.ShowError();
        return;
    }

    wxString fbreader = FbParams().GetText(FB_FB2_PROGRAM);

    wxFileName file_name = wxFileName::CreateTempFileName(wxT("~"));
    wxRemoveFile(file_name.GetFullPath());
    file_name.SetExt(wxT("fb2"));
    wxString file_path = file_name.GetFullPath();
    TempFileEraser::Add(file_path);
    wxFileOutputStream out(file_path);
    out.Write(reader.GetZip());

    #if defined(__WXMSW__)
    ShellExecute(NULL, NULL, fbreader, file_path, NULL, SW_SHOW);
    #else
    wxExecute(fbreader + wxT(" ") + file_path);
    #endif
}

void BookInfo::MakeLower(wxString & data){
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

void BookInfo::MakeUpper(wxString & data){
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

int BookInfo::NewId(int param)
{
	Params params(wxGetApp().GetDatabase());
	ParamsRow * row = params.Id(param);
	row->value++;
	row->Save();

	return row->value;
}

