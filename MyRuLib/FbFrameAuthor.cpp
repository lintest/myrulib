#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbAuthorThread.h"
#include "FbAuthorDlg.h"
#include "FbReplaceDlg.h"
#include "FbUpdateThread.h"
#include "FbMasterList.h"

BEGIN_EVENT_TABLE(FbFrameAuthor, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameAuthor::OnMasterSelected)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameAuthor::OnColClick)
	EVT_MENU(wxID_SAVE, FbFrameAuthor::OnExternal)
	EVT_KEY_UP(FbFrameAuthor::OnCharEvent)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameAuthor::OnBooksCount)
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameAuthor::OnContextMenu)
	EVT_MENU(ID_LETTER_ALL, FbFrameAuthor::OnAllClicked)
	EVT_MENU(ID_MASTER_APPEND, FbFrameAuthor::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbFrameAuthor::OnMasterModify)
	EVT_MENU(ID_MASTER_REPLACE, FbFrameAuthor::OnMasterReplace)
	EVT_MENU(ID_MASTER_DELETE, FbFrameAuthor::OnMasterDelete)
	EVT_MENU(ID_MASTER_PAGE, FbFrameAuthor::OnMasterPage)
	EVT_MENU(ID_ALPHABET_RU, FbFrameAuthor::OnViewAlphavet)
	EVT_MENU(ID_ALPHABET_EN, FbFrameAuthor::OnViewAlphavet)
	EVT_UPDATE_UI(ID_ALPHABET_RU, FbFrameAuthor::OnViewAlphavetUpdateUI)
	EVT_UPDATE_UI(ID_ALPHABET_EN, FbFrameAuthor::OnViewAlphavetUpdateUI)
	EVT_UPDATE_UI(ID_MASTER_PAGE, FbFrameAuthor::OnMasterPageUpdateUI)
END_EVENT_TABLE()

FbFrameAuthor::FbFrameAuthor(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_AUTHOR, GetTitle())
{
	CreateControls();
}

void FbFrameAuthor::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	sizer->Add(m_RuAlphabar = CreateAlphaBar(this, ID_ALPHABET_RU, alphabetRu, ID_LETTER_RU, wxTB_NODIVIDER), 0, wxEXPAND, 5);
	sizer->Add(m_EnAlphabar = CreateAlphaBar(this, ID_ALPHABET_EN, alphabetEn, ID_LETTER_EN, wxTB_NODIVIDER), 0, wxEXPAND, 5);

	wxFont font = FbParams::GetFont(FB_FONT_TOOL);
	m_RuAlphabar->SetFont(font);
	m_EnAlphabar->SetFont(font);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbMasterList(splitter, ID_MASTER_LIST, wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER);
	m_MasterList->AddColumn(_("Author"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(_("Num."), 10, wxALIGN_RIGHT);
	m_MasterList->SetFocus();
	m_MasterList->SetSortedColumn(1);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	m_RuAlphabar->Show( FbParams::GetValue(FB_ALPHABET_RU) );
	m_EnAlphabar->Show( FbParams::GetValue(FB_ALPHABET_EN) );
}

void FbFrameAuthor::Localize(bool bUpdateMenu)
{
	FbFrameBase::Localize(bUpdateMenu);
	m_MasterList->SetColumnText(0, _("Author"));
	m_MasterList->SetColumnText(1, _("Num."));
}

wxToolBar * FbFrameAuthor::CreateAlphaBar(wxWindow * parent, wxWindowID id, const wxString & alphabet, const int &toolid, long style)
{
	wxToolBar * toolbar = new wxToolBar(parent, id, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT|wxTB_NOICONS|style);
	toolbar->SetToolBitmapSize(wxSize(0,0));
	if (toolid == ID_LETTER_EN) toolbar->AddTool(ID_LETTER_ALL, wxT("*"), wxNullBitmap, wxNullBitmap, wxITEM_CHECK, _("All collected authors"));
	for (size_t i = 0; i<alphabet.Len(); i++) {
		wxString letter = alphabet.Mid(i, 1);
		int btnid = toolid + i;
		toolbar->AddTool(btnid, letter, wxNullBitmap, wxNullBitmap, wxITEM_CHECK);
		this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( FbFrameAuthor::OnLetterClicked ) );
	}
	toolbar->Realize();
	return toolbar;
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
	m_EnAlphabar->ToggleTool(ID_LETTER_ALL, ID_LETTER_ALL == idLetter);
}

void FbFrameAuthor::OnLetterClicked( wxCommandEvent& event )
{
	int id = event.GetId();

	wxString alphabet;
	size_t position;

	if (id < ID_LETTER_EN) {
		alphabet = alphabetRu;
		position = id - ID_LETTER_RU;
		FbParams().SetValue(FB_LAST_LETTER, position);
	} else {
		alphabet = alphabetEn;
		position = id - ID_LETTER_EN;
	};

	ToggleAlphabar(id);

	(new FbAuthorThreadChar(m_MasterList, alphabet[position], m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_CHAR;
	m_AuthorText = alphabet[position];
}

void FbFrameAuthor::OnAllClicked(wxCommandEvent& event)
{
	wxString text = wxT("*");
	FbFrameAuthor::ToggleAlphabar(ID_LETTER_ALL);
	(new FbAuthorThreadText(m_MasterList, text, m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_TEXT;
	m_AuthorText = text;
	FbParams().SetValue(FB_LAST_LETTER, -1);
}

void FbFrameAuthor::SelectFirstAuthor(const int book)
{
	m_BooksPanel->EmptyBooks(book);

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_MasterList->GetFirstChild(m_MasterList->GetRootItem(), cookie);
	if (item.IsOk()) m_MasterList->SelectItem(item);
}

void FbFrameAuthor::OnMasterSelected(wxTreeEvent & event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data) data->Show(this);
}

void FbFrameAuthor::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameAuthor::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
	ToggleAlphabar(0);
	(new FbAuthorThreadText(m_MasterList, text, m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_TEXT;
	m_AuthorText = text;
}

void FbFrameAuthor::OpenAuthor(const int author, const int book)
{
	ToggleAlphabar(0);
	(new FbAuthorThreadCode(m_MasterList, author, m_MasterList->GetSortedColumn()))->Execute();
	m_AuthorMode = FB_AUTHOR_MODE_CODE;
	m_AuthorCode = author;
}

void FbFrameAuthor::SelectRandomLetter()
{
	int position = FbParams().GetValue(FB_LAST_LETTER);
	wxCommandEvent event( wxEVT_COMMAND_TOOL_CLICKED, ID_LETTER_RU + position );
	wxPostEvent(this, event);
}

void FbFrameAuthor::OnExternal(wxCommandEvent& event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data) ExternalDlg::Execute(this, m_BooksPanel->m_BookList, data->GetId());
}

void FbFrameAuthor::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data) data->Show(this);
}

void FbFrameAuthor::OnCharEvent(wxKeyEvent& event)
{
}

void FbFrameAuthor::OnColClick(wxListEvent& event)
{
	int order = m_MasterList->GetSortedColumn();
	FbThread * thread = NULL;
	switch (m_AuthorMode) {
		case FB_AUTHOR_MODE_CHAR: thread = new FbAuthorThreadChar(m_MasterList, m_AuthorText, order); break;
		case FB_AUTHOR_MODE_TEXT: thread = new FbAuthorThreadText(m_MasterList, m_AuthorText, order); break;
		case FB_AUTHOR_MODE_CODE: thread = new FbAuthorThreadCode(m_MasterList, m_AuthorCode, order); break;
	}
	if (thread) thread->Execute();
}

void FbFrameAuthor::OnBooksCount(wxCommandEvent& event)
{
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) m_MasterList->SetItemText(selected, 1, wxString::Format(wxT("%d "), GetBookCount()));
	event.Skip();
}

void FbFrameAuthor::ShowFullScreen(bool show)
{
	if (m_RuAlphabar) m_RuAlphabar->Show(!show && FbParams::GetValue(FB_ALPHABET_RU));
	if (m_EnAlphabar) m_EnAlphabar->Show(!show && FbParams::GetValue(FB_ALPHABET_EN));
	Layout();
}

void FbFrameAuthor::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameAuthor::ShowContextMenu(const wxPoint& pos, wxTreeItemId item)
{
	int id = 0;
	if (item.IsOk()) {
		FbMasterData * data = m_MasterList->GetItemData(item);
		if (data) id = data->GetId();
	}
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameAuthor::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbAuthorDlg::Append(newname);

	if (id) {
		FbMasterData * data = new FbMasterAuthor(id);
		wxTreeItemId item = m_MasterList->InsertItem(m_MasterList->GetRootItem(), m_MasterList->GetSelection(), newname, -1, -1, data);
		m_MasterList->SelectItem(item);
	}
}

void FbFrameAuthor::OnMasterModify(wxCommandEvent& event)
{
    wxTreeItemId selected = m_MasterList->GetSelection();
	FbMasterData * data = selected.IsOk() ? m_MasterList->GetItemData(selected) : NULL;
	if (data && data->GetId()) {
		wxString newname;
		int old_id = data->GetId();
		int new_id = FbAuthorDlg::Modify(data->GetId(), newname);
		if (new_id) ReplaceData(old_id, new_id, selected, newname);
	}
}

void FbFrameAuthor::OnMasterReplace(wxCommandEvent& event)
{
    wxTreeItemId selected = m_MasterList->GetSelection();
	FbMasterData * data = selected.IsOk() ? m_MasterList->GetItemData(selected) : NULL;
	if (data && data->GetId()) {
		wxString newname;
		int old_id = data->GetId();
		int new_id = FbReplaceDlg::Execute(old_id, newname);
		if (new_id) ReplaceData(old_id, new_id, selected, newname);
	}
}

void FbFrameAuthor::ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname)
{
	if (selected.IsOk()) m_MasterList->SetItemText(selected, newname);
	if (old_id != new_id) {
		FbMasterAuthor deleted(new_id);
		m_MasterList->DeleteItem(deleted);
		delete m_MasterList->GetItemData(selected);
		FbMasterData * data = new FbMasterAuthor(new_id);
		m_MasterList->SetItemData(selected, data);
		m_MasterList->SelectItem(selected);
	}
}

void FbFrameAuthor::OnMasterDelete(wxCommandEvent& event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (!data) return;
	int id = data->GetId();
	if (!id) return;

	wxString sql = wxT("SELECT count(id) FROM books WHERE id_author=?");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	int count = result.NextRow() ? result.GetInt(0) : 0;

	wxTreeItemId selected = m_MasterList->GetSelection();
	wxString msg = _("Delete author") + COLON + m_MasterList->GetItemText(selected);
	if (count) msg += (wxString)wxT("\n") +  wxString::Format(_("and all of author's books (%d pcs.)?"), count);
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		wxString sql1 = wxString::Format(wxT("DELETE FROM books WHERE id_author=%d"), id);
		wxString sql2 = wxString::Format(wxT("DELETE FROM authors WHERE id=%d"), id);
		(new FbUpdateThread(sql1, sql2))->Execute();
		m_MasterList->Delete(selected);
	}
}

void FbFrameAuthor::OnMasterPage(wxCommandEvent& event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	if (data && data->GetId()>0) {
		wxString host = FbParams::GetText(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/a/%d"), host.c_str(), data->GetId());
		wxLaunchDefaultBrowser(url);
	}
}

FbFrameAuthor::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_REPLACE, _("Replace"));
	Append(ID_MASTER_DELETE,  _("Delete"));
	if (id > 0) {
		AppendSeparator();
		Append(ID_MASTER_PAGE, _("Online authors page"));
	}
}

FbFrameAuthor::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("&File"));
	Append(new MenuLib,    _("&Library"));
	Append(new MenuFrame,  _("&Catalog"));
	Append(new MenuMaster, _("&Authors"));
	Append(new MenuBook,   _("&Books"));
	Append(new MenuView,   _("&View"));
	Append(new MenuSetup,  _("&Tools"));
	Append(new MenuWindow, _("&Window"));
	Append(new MenuHelp,   _("&?"));
}

FbFrameAuthor::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND,  _("Append"));
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_REPLACE, _("Replace"));
	Append(ID_MASTER_DELETE,  _("Delete"));
	AppendSeparator();
	Append(ID_MASTER_PAGE, _("Online authors page"));
}

wxMenuBar * FbFrameAuthor::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameAuthor::OnMasterPageUpdateUI(wxUpdateUIEvent & event)
{
	FbMasterData * data = m_MasterList->GetSelectedData();
	event.Enable( data && data->GetId()>0 );
}

FbFrameAuthor::MenuBar::MenuView::MenuView()
{
	InsertCheckItem(0, ID_ALPHABET_RU, _("Russian aplhabet"));
	InsertCheckItem(1, ID_ALPHABET_EN, _("Latin alphabet"));
	InsertSeparator(2);
}

void FbFrameAuthor::OnViewAlphavet(wxCommandEvent& event)
{
	wxToolBar * control = (wxToolBar*) FindWindowById(event.GetId());
	if (!control) return;

	bool show = ! control->IsShown();
	control->Show(show);
	Layout();

	int key = 0;
	switch ( event.GetId() ) {
		case ID_ALPHABET_RU: key = FB_ALPHABET_RU; break;
		case ID_ALPHABET_EN: key = FB_ALPHABET_EN; break;
	}
	if (key) FbParams().SetValue(key, show);
}

void FbFrameAuthor::OnViewAlphavetUpdateUI(wxUpdateUIEvent & event)
{
	wxToolBar * control = (wxToolBar*) FindWindowById(event.GetId());
	event.Check(control->IsShown());
}

