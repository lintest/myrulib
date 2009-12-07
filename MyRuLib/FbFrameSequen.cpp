#include "FbFrameSequen.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbManager.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"

BEGIN_EVENT_TABLE(FbFrameSequen, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameSequen::OnAuthorSelected)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameSequen::OnColClick)
	EVT_COMMAND(ID_EMPTY_AUTHORS, fbEVT_AUTHOR_ACTION, FbFrameSequen::OnEmptyAuthors)
	EVT_FB_AUTHOR(ID_APPEND_AUTHOR, FbFrameSequen::OnAppendAuthor)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameSequen::OnBooksCount)
	EVT_TEXT_ENTER(ID_SEQUENCE_TXT, FbFrameSequen::OnFindEnter )
	EVT_MENU(ID_SEQUENCE_BTN, FbFrameSequen::OnFindEnter )
END_EVENT_TABLE()

FbFrameSequen::FbFrameSequen(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_AUTHOR, _("Серии"))
{
	CreateControls();
}

void FbFrameSequen::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	wxBoxSizer* bSizerSeq;
	bSizerSeq = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * text = new wxStaticText( this, wxID_ANY, _("Серия:"), wxDefaultPosition, wxDefaultSize, 0 );
	text->Wrap( -1 );
	bSizerSeq->Add( text, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_FindText = new wxTextCtrl( this, ID_SEQUENCE_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );

	bSizerSeq->Add( m_FindText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizerSeq->Add( toolbar, 1, wxALIGN_CENTER_VERTICAL);

	sizer->Add(bSizerSeq, 0, wxEXPAND, 5);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbTreeListCtrl(splitter, ID_MASTER_LIST, wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER);
	m_MasterList->AddColumn(_("Серия"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(_("Кол."), 10, wxALIGN_RIGHT);
	m_MasterList->SetFocus();
	m_MasterList->SetSortedColumn(1);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	FindSequence(wxEmptyString);
}

wxToolBar * FbFrameSequen::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(FbParams::GetFont(FB_FONT_TOOL));
	toolbar->AddTool(ID_SEQUENCE_BTN, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Найти серию по наименованию"));
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameSequen::SelectFirstAuthor(const int book)
{
	m_BooksPanel->EmptyBooks(book);

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_MasterList->GetFirstChild(m_MasterList->GetRootItem(), cookie);
	if (item.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameSequen::OnAuthorSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_MasterList->GetItemData(selected);
		if (data) ( new SequenThread(this, m_BooksPanel->GetListMode(), data->GetId()) )->Execute();
	}
}

void FbFrameSequen::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameSequen::FindSequence(const wxString &text)
{
	m_SequenceText = text;
	(new MasterThread(this, m_SequenceText, m_MasterList->GetSortedColumn()))->Execute();
}

void FbFrameSequen::OpenSequence(const int sequence, const int book)
{
}

void FbFrameSequen::SelectRandomLetter()
{
	wxDateTime now = wxDateTime::Now();
	int random = now.GetHour() * 60 * 60 + now.GetMinute() * 60 + now.GetSecond();
	random = random % alphabetRu.Len();

	wxCommandEvent event( wxEVT_COMMAND_TOOL_CLICKED, ID_LETTER_RU + random );
	wxPostEvent(this, event);
}

FbThreadSkiper FbFrameSequen::SequenThread::sm_skiper;

void * FbFrameSequen::SequenThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;

	try {
		FbCommonDatabase database;
		InitDatabase(database);

		wxString condition = wxT("books.id IN (SELECT id_book FROM bookseq WHERE id_seq=?)");
		if (m_mode == FB2_MODE_TREE) condition += wxT("AND bookseq.id_seq=?");
		wxString sql = GetSQL(condition);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_master);
		if (m_mode == FB2_MODE_TREE) stmt.Bind(2, m_master);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbFrameSequen::UpdateBooklist()
{
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbAuthorData * data = (FbAuthorData*) m_MasterList->GetItemData(selected);
		if (data) (new SequenThread(this, m_BooksPanel->GetListMode(), data->GetId()))->Execute();
	}
}

void FbFrameSequen::OnAppendAuthor(FbAuthorEvent& event)
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

void FbFrameSequen::OnEmptyAuthors(wxCommandEvent& event)
{
	BookListUpdater updater(m_MasterList);
	m_MasterList->AddRoot(wxEmptyString);
}

void FbFrameSequen::OnColClick(wxListEvent& event)
{
	(new MasterThread(this, m_SequenceText, m_MasterList->GetSortedColumn()))->Execute();
}

void FbFrameSequen::OnBooksCount(wxCommandEvent& event)
{
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d"), GetBookCount()));
	event.Skip();
}

wxCriticalSection FbFrameSequen::MasterThread::sm_queue;

void * FbFrameSequen::MasterThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	try {
		FbCommonDatabase database;
		FbSearchFunction search(m_text);
		wxString sql = wxT("SELECT id, value as name, number FROM sequences");
		if (!m_text.IsEmpty()) {
			sql += wxT(" WHERE SEARCH(value)");
			database.CreateFunction(wxT("SEARCH"), 1, search);
		}
		sql += wxT(" ORDER BY ") + GetOrder();
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		FbCommandEvent(fbEVT_AUTHOR_ACTION, ID_EMPTY_AUTHORS).Post(m_frame);
		while (result.NextRow()) {
			FbAuthorEvent(ID_APPEND_AUTHOR, result).Post(m_frame);
		}
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

wxString FbFrameSequen::MasterThread::GetOrder()
{
	switch (m_order) {
		case -2: return wxT("number desc, name desc");
		case -1: return wxT("name desc");
		case  2: return wxT("number, name");
		default: return wxT("name ");
	}
}

void FbFrameSequen::OnFindEnter(wxCommandEvent& event)
{
	FindSequence(m_FindText->GetValue());
}
