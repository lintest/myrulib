#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbFrameBaseThread.h"

BEGIN_EVENT_TABLE(FbFrameAuthor, FbFrameBase)
    EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameAuthor::OnAuthorSelected)
    EVT_MENU(wxID_SAVE, FbFrameAuthor::OnExternal)
END_EVENT_TABLE()

class FrameAuthorThread: public FbFrameBaseThread
{
    public:
        FrameAuthorThread(FbFrameBase * frame, FbListMode mode, const int author)
			:FbFrameBaseThread(frame, mode), m_author(author), m_number(sm_skiper.NewNumber()) {};
        virtual void *Entry();
    protected:
		virtual void CreateTree(wxSQLite3ResultSet &result);
		virtual wxString GetSQL(const wxString & condition);
    private:
		static FbThreadSkiper sm_skiper;
        int m_author;
        int m_number;
};

FbFrameAuthor::FbFrameAuthor(wxAuiMDIParentFrame * parent)
    :FbFrameBase(parent, ID_FRAME_AUTHOR, _("Авторы"))
{
    CreateControls();
}

void FbFrameAuthor::CreateControls()
{
	SetMenuBar(new FbFrameBaseMenu);

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(m_RuAlphabar = CreateAlphaBar(this, alphabetRu, ID_LETTER_RU, wxTB_NODIVIDER), 0, wxEXPAND, 5);
	sizer->Add(m_EnAlphabar = CreateAlphaBar(this, alphabetEn, ID_LETTER_EN, 0), 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_AuthorList = new FbAuthorList(splitter, ID_MASTER_LIST);
	m_AuthorList->SetFocus();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_AuthorList, &m_BooksPanel, 160);

	Layout();
}

wxToolBar * FbFrameAuthor::CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style)
{
	wxToolBar * toolBar = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NOICONS|style);
	for (size_t i = 0; i<alphabet.Len(); i++) {
	    wxString letter = alphabet.Mid(i, 1);
	    int btnid = toolid + i;
        toolBar->AddTool(btnid, letter, wxNullBitmap, wxNullBitmap, wxITEM_CHECK)->SetClientData( (wxObject*) i);
        this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( FbFrameAuthor::OnLetterClicked ) );
	}
	toolBar->Realize();
	return toolBar;
}

void FbFrameAuthor::ToggleAlphabar(const int &idLetter)
{
	for (size_t i = 0; i<alphabetRu.Len(); i++) {
	    int id = ID_LETTER_RU + i;
        m_RuAlphabar->ToggleTool(id, id == idLetter);
	}
	for (size_t i = 0; i<alphabetEn.Len(); i++) {
	    int id = ID_LETTER_EN + i;
        m_EnAlphabar->ToggleTool(id, id == idLetter);
	}
}

void FbFrameAuthor::OnLetterClicked( wxCommandEvent& event )
{
    int id = event.GetId();

    wxString alphabet;
	size_t position;

    if (id < ID_LETTER_EN) {
        alphabet = alphabetRu;
        position = id - ID_LETTER_RU;
    } else {
        alphabet = alphabetEn;
        position = id - ID_LETTER_EN;
    };

    ToggleAlphabar(id);

	m_AuthorList->FillAuthorsChar(alphabet[position]);
	SelectFirstAuthor();
}

void FbFrameAuthor::SelectFirstAuthor()
{
	m_BooksPanel.EmptyBooks();

    wxTreeItemIdValue cookie;
    wxTreeItemId item = m_AuthorList->GetFirstChild(m_AuthorList->GetRootItem(), cookie);
	if (item.IsOk()) {
		m_AuthorList->SelectItem(item);
		FbAuthorData * data = (FbAuthorData*) m_AuthorList->GetItemData(item);
        if (data) {
			wxThread * thread = new FrameAuthorThread(this, m_BooksPanel.GetListMode(), data->GetId());
			if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
        }
	}
}

void FbFrameAuthor::OnAuthorSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
        m_BooksPanel.EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_AuthorList->GetItemData(selected);
		if (data) {
			wxThread * thread = new FrameAuthorThread(this, m_BooksPanel.GetListMode(), data->GetId());
			if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
		}
	}
}

void FbFrameAuthor::ActivateAuthors()
{
    m_AuthorList->SetFocus();
}

void FbFrameAuthor::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
    ToggleAlphabar(0);
    m_AuthorList->FillAuthorsText(text);
    SelectFirstAuthor();
}

void FbFrameAuthor::OpenAuthor(const int id)
{
	if (id == 0) return;
    ToggleAlphabar(0);
    m_AuthorList->FillAuthorsCode(id);
    SelectFirstAuthor();
}

void FbFrameAuthor::SelectRandomLetter()
{
    wxDateTime now = wxDateTime::Now();
    int random = now.GetHour() * 60 * 60 + now.GetMinute() * 60 + now.GetSecond();
	random = random % alphabetRu.Len();

    wxCommandEvent event( wxEVT_COMMAND_TOOL_CLICKED, ID_LETTER_RU + random );
    wxPostEvent(this, event);
}

void FbFrameAuthor::OnExternal(wxCommandEvent& event)
{
    wxTreeItemId item = m_AuthorList->GetSelection();
    if (item.IsOk()) {
		FbAuthorData * data = (FbAuthorData*) m_AuthorList->GetItemData(item);
		if (data) ExternalDlg::Execute(this, m_BooksPanel.m_BookList, data->GetId());
    }
}

FbThreadSkiper FrameAuthorThread::sm_skiper;

wxString FrameAuthorThread::GetSQL(const wxString & condition)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE:
			sql = wxT("\
				SELECT (CASE WHEN bookseq.id_seq IS NULL THEN 1 ELSE 0 END) AS key, \
					books.id, books.title, books.file_size, books.file_type, books.id_author, \
					states.rating, sequences.value AS sequence, bookseq.number as number\
				FROM books \
					LEFT JOIN bookseq ON bookseq.id_book=books.id AND bookseq.id_author = books.id_author \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
                WHERE (%s) \
				ORDER BY key, sequences.value, bookseq.number, books.title \
			"); break;
		case FB2_MODE_LIST:
			sql = wxT("\
				SELECT \
					books.id as id, books.title as title, books.file_size as file_size, books.file_type as file_type, \
					states.rating, authors.full_name as full_name, 0 as number \
				FROM books \
					LEFT JOIN books as sub ON sub.id=books.id \
					LEFT JOIN authors ON sub.id_author = authors.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
                WHERE (%s) \
				ORDER BY books.title, books.id, authors.full_name\
			"); break;
	}

	wxString str = wxT("(%s)");
	if (m_FilterFb2) str += wxT("AND(books.file_type='fb2')");
	if (m_FilterLib) str += wxT("AND(books.id>0)");
	if (m_FilterUsr) str += wxT("AND(books.id<0)");
	sql = wxString::Format(sql, str.c_str());

	return wxString::Format(sql, condition.c_str());
}

void * FrameAuthorThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	try {
		FbCommonDatabase database;
		database.AttachConfig();
		if (m_mode == FB2_MODE_TREE) {
            wxString sql = wxT("SELECT full_name FROM authors WHERE id=?");
            wxSQLite3Statement stmt = database.PrepareStatement(sql);
            stmt.Bind(1, m_author);
            wxSQLite3ResultSet result = stmt.ExecuteQuery();
            if (result.NextRow()) {
                wxString thisAuthor = result.GetString(wxT("full_name"));
                wxCommandEvent event(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR);
                event.SetString(thisAuthor);
                wxPostEvent(m_frame, event);
            }
		}

        wxString sql = GetSQL(wxT("books.id_author=?"));
        FbGenreFunction function;
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, m_author);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();

        if (sm_skiper.Skipped(m_number)) return NULL;
        FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FrameAuthorThread::CreateTree(wxSQLite3ResultSet &result)
{
    wxString thisSequence = wxT("@@@");
    while (result.NextRow()) {
	    wxString nextSequence = result.GetString(wxT("sequence"));

	    if (thisSequence != nextSequence) {
	        thisSequence = nextSequence;
			wxCommandEvent event(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE);
			event.SetString(thisSequence);
			wxPostEvent(m_frame, event);
	    }

        BookTreeItemData data(result);
		FbBookEvent event(fbEVT_BOOK_ACTION, ID_APPEND_BOOK, &data);
		wxPostEvent(m_frame, event);
    }
}

void FbFrameAuthor::UpdateBooklist()
{
	wxTreeItemId selected = m_AuthorList->GetSelection();
	if (selected.IsOk()) {
        m_BooksPanel.EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_AuthorList->GetItemData(selected);
		if (data) {
			wxThread * thread = new FrameAuthorThread(this, m_BooksPanel.GetListMode(), data->GetId());
			if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
		}
	}
}
