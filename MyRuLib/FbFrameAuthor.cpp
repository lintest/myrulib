#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbAuthorThread.h"

BEGIN_EVENT_TABLE(FbFrameAuthor, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameAuthor::OnAuthorSelected)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameAuthor::OnColClick)
	EVT_MENU(wxID_SAVE, FbFrameAuthor::OnExternal)
	EVT_KEY_UP(FbFrameAuthor::OnCharEvent)
	EVT_COMMAND(ID_EMPTY_AUTHORS, fbEVT_AUTHOR_ACTION, FbFrameAuthor::OnEmptyAuthors)
	EVT_FB_AUTHOR(ID_APPEND_AUTHOR, FbFrameAuthor::OnAppendAuthor)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameAuthor::OnBooksCount)
END_EVENT_TABLE()

FbFrameAuthor::FbFrameAuthor(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_AUTHOR, _("Авторы"))
{
	CreateControls();
}

void FbFrameAuthor::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(m_RuAlphabar = CreateAlphaBar(this, alphabetRu, ID_LETTER_RU, wxTB_NODIVIDER), 0, wxEXPAND, 5);
	sizer->Add(m_EnAlphabar = CreateAlphaBar(this, alphabetEn, ID_LETTER_EN, 0), 0, wxEXPAND, 5);

	wxFont font = FbParams::GetFont(FB_FONT_TOOL);
	m_RuAlphabar->SetFont(font);
	m_EnAlphabar->SetFont(font);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbAuthorList(splitter, ID_MASTER_LIST);
	m_MasterList->SetFocus();
	m_MasterList->SetSortedColumn(1);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();
}

wxToolBar * FbFrameAuthor::CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style)
{
	wxToolBar * toolBar = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NOICONS|style);
	for (size_t i = 0; i<alphabet.Len(); i++) {
		wxString letter = alphabet.Mid(i, 1);
		int btnid = toolid + i;
		toolBar->AddTool(btnid, letter, wxNullBitmap, wxNullBitmap, wxITEM_CHECK);
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

	(new FbAuthorThreadChar(this, alphabet[position], m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_CHAR;
	m_AuthorText = alphabet[position];
}

void FbFrameAuthor::SelectFirstAuthor(const int book)
{
	m_BooksPanel->EmptyBooks(book);

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_MasterList->GetFirstChild(m_MasterList->GetRootItem(), cookie);
	if (item.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameAuthor::OnAuthorSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_MasterList->GetItemData(selected);
		if (data) ( new AuthorThread(this, m_BooksPanel->GetListMode(), data->GetId()) )->Execute();
	}
}

void FbFrameAuthor::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameAuthor::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
	ToggleAlphabar(0);
	(new FbAuthorThreadText(this, text, m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_TEXT;
	m_AuthorText = text;
}

void FbFrameAuthor::OpenAuthor(const int author, const int book)
{
	ToggleAlphabar(0);
	(new FbAuthorThreadCode(this, author, m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_CODE;
	m_AuthorCode = author;
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
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) {
		FbAuthorData * data = (FbAuthorData*) m_MasterList->GetItemData(item);
		if (data) ExternalDlg::Execute(this, m_BooksPanel->m_BookList, data->GetId());
	}
}

FbThreadSkiper FbFrameAuthor::AuthorThread::sm_skiper;

wxString FbFrameAuthor::AuthorThread::GetSQL(const wxString & condition)
{
	wxString sql;
	switch (m_mode) {
		case FB2_MODE_TREE:
			sql = wxT("\
				SELECT DISTINCT (CASE WHEN bookseq.id_seq IS NULL THEN 1 ELSE 0 END) AS key, \
					books.id, books.title, books.file_size, books.file_type, books.id_author, \
					states.rating, sequences.value AS sequence, bookseq.number as number\
				FROM books \
					LEFT JOIN bookseq ON bookseq.id_book=books.id \
					LEFT JOIN sequences ON bookseq.id_seq=sequences.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				ORDER BY key, sequences.value, bookseq.number, books.title \
			");
			break;
		case FB2_MODE_LIST:
			sql = wxT("\
				SELECT DISTINCT\
					books.id as id, books.title as title, books.file_size as file_size, books.file_type as file_type, \
					states.rating as rating, books.created as created, AGGREGATE(authors.full_name) as full_name \
				FROM books \
					LEFT JOIN books as sub ON sub.id=books.id \
					LEFT JOIN authors ON sub.id_author = authors.id \
					LEFT JOIN states ON books.md5sum=states.md5sum \
				WHERE (%s) \
				GROUP BY books.id, books.title, books.file_size, books.file_type, states.rating, books.created \
				ORDER BY \
			") + GetOrder();
			break;
	}

	wxString str = wxT("(%s)");
	if (m_FilterFb2) str += wxT("AND(books.file_type='fb2')");
	if (m_FilterLib) str += wxT("AND(books.id>0)");
	if (m_FilterUsr) str += wxT("AND(books.id<0)");
	sql = wxString::Format(sql, str.c_str());

	return wxString::Format(sql, condition.c_str());
}

void * FbFrameAuthor::AuthorThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;

	try {
		FbCommonDatabase database;
		InitDatabase(database);

		if (m_mode == FB2_MODE_TREE) {
			wxString sql = wxT("SELECT full_name FROM authors WHERE id=?");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, m_author);
			wxSQLite3ResultSet result = stmt.ExecuteQuery();
			if (result.NextRow()) {
				wxString thisAuthor = result.GetString(wxT("full_name"));
				FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_AUTHOR, thisAuthor).Post(m_frame);
			}
		}

		wxString sql = GetSQL(wxT("books.id_author=?"));
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

void FbFrameAuthor::AuthorThread::CreateTree(wxSQLite3ResultSet &result)
{
	wxString thisSequence = wxT("@@@");
	while (result.NextRow()) {
		wxString nextSequence = result.GetString(wxT("sequence"));

		if (thisSequence != nextSequence) {
			thisSequence = nextSequence;
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_APPEND_SEQUENCE, thisSequence).Post(m_frame);
		}

		BookTreeItemData data(result);
		FbBookEvent(ID_APPEND_BOOK, &data).Post(m_frame);
	}
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_BOOKS_COUNT).Post(m_frame);
}

void FbFrameAuthor::UpdateBooklist()
{
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_MasterList->GetItemData(selected);
		if (data) (new AuthorThread(this, m_BooksPanel->GetListMode(), data->GetId()))->Execute();
	}
}

void FbFrameAuthor::OnCharEvent(wxKeyEvent& event)
{
}

void FbFrameAuthor::OnAppendAuthor(FbAuthorEvent& event)
{
	FbTreeListUpdater updater(m_MasterList);
	wxTreeItemId root = m_MasterList->GetRootItem();

	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_MasterList->GetFirstChild(root, cookie);

	wxTreeItemId item = m_MasterList->AppendItem(root, event.GetString(), -1, -1, new FbAuthorData(event.m_author));
	wxString number = wxString::Format(wxT("%d"), event.m_number);
	m_MasterList->SetItemText(item, 1, number);

	if (!child.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameAuthor::OnEmptyAuthors(wxCommandEvent& event)
{
	BookListUpdater updater(m_MasterList);
	m_MasterList->AddRoot(wxEmptyString);
}

void FbFrameAuthor::OnColClick(wxListEvent& event)
{
	int order = m_MasterList->GetSortedColumn();
	FbThread * thread = NULL;
	switch (m_AuthorMode) {
		case FB_AUTHOR_MODE_CHAR: thread = new FbAuthorThreadChar(this, m_AuthorText, order); break;
		case FB_AUTHOR_MODE_TEXT: thread = new FbAuthorThreadText(this, m_AuthorText, order); break;
		case FB_AUTHOR_MODE_CODE: thread = new FbAuthorThreadCode(this, m_AuthorCode, order); break;
	}
	if (thread) thread->Execute();
}

void FbFrameAuthor::OnBooksCount(wxCommandEvent& event)
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d"), GetBookCount()));
	event.Skip();
}

