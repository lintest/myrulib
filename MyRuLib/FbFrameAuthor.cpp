#include "FbFrameAuthor.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbAuthorDlg.h"
#include "FbReplaceDlg.h"
#include "FbUpdateThread.h"
#include "FbMasterList.h"

IMPLEMENT_CLASS(FbFrameAuthor, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameAuthor, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameAuthor::OnMasterSelected)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameAuthor::OnColClick)
	EVT_MENU(wxID_SAVE, FbFrameAuthor::OnExternal)
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
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameAuthor::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameAuthor::OnArray)
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

	m_MasterList = new FbTreeViewCtrl(splitter, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxLC_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

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
	m_MasterList->EmptyColumns();
	CreateColumns();
	m_MasterList->Refresh();
}

void FbFrameAuthor::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Author"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
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

	m_info = (wxChar)alphabet[position];
	CreateMasterThread();
}

void FbFrameAuthor::OnAllClicked(wxCommandEvent& event)
{
	wxString text = wxT("*");
	FbFrameAuthor::ToggleAlphabar(ID_LETTER_ALL);
	FbParams().SetValue(FB_LAST_LETTER, -1);
	m_info = wxChar(0);
	CreateMasterThread();
}

void FbFrameAuthor::CreateMasterThread()
{
	wxDELETE(m_MasterLocker);
	m_MasterLocker = new FbMutexLocker;
	if (m_MasterThread) m_MasterThread->Wait();
	wxDELETE(m_MasterThread);
	m_MasterThread = new FbAuthListThread(this, *m_MasterLocker, m_info, m_MasterList->GetSortedColumn());
	m_MasterThread->Execute();
}

void FbFrameAuthor::OnMasterSelected(wxTreeEvent & event)
{
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (data) FbMasterAuthor(data->GetCode()).Show(this);
}

void FbFrameAuthor::ActivateAuthors()
{
	m_MasterList->SetFocus();
}

void FbFrameAuthor::FindAuthor(const wxString &text)
{
	if (text.IsEmpty()) return;
	ToggleAlphabar(0);
	m_info = text;
	CreateMasterThread();
}

void FbFrameAuthor::OpenAuthor(const int author, const int book)
{
	if (author == 0) return;
	ToggleAlphabar(0);
	m_info = author;
	CreateMasterThread();
}

void FbFrameAuthor::SelectRandomLetter()
{
	int position = FbParams().GetValue(FB_LAST_LETTER);
	FbCommandEvent(wxEVT_COMMAND_TOOL_CLICKED, ID_LETTER_RU + position).Post(this);
}

void FbFrameAuthor::OnExternal(wxCommandEvent& event)
{
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (data) ExternalDlg::Execute(this, m_BooksPanel->m_BookList, data->GetCode());
}

void FbFrameAuthor::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (data) FbMasterAuthor(data->GetCode()).Show(this);
}

void FbFrameAuthor::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameAuthor::OnBooksCount(wxCommandEvent& event)
{
/*
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) m_MasterList->SetItemText(selected, 1, wxString::Format(wxT("%d "), GetBookCount()));
	event.Skip();
*/
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
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (data) FbMasterAuthor(data->GetCode()).Show(this);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameAuthor::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbAuthorDlg::Append(newname);
	if (id == 0) return;

	FbCacheData * cache = new FbCacheData(id, newname);
	FbCollection::AddAuth(cache);

	m_MasterList->Append(new FbAuthListData(id));
}

void FbFrameAuthor::OnMasterModify(wxCommandEvent& event)
{
	FbAuthListData * current = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (current == NULL) return;

	wxString newname;
	int id = FbAuthorDlg::Modify(current->GetCode(), newname);
	if (id == 0) return;

	FbCacheData * cache = new FbCacheData(id, newname);
	FbCollection::AddAuth(cache);

	m_MasterList->Replace(new FbAuthListData(id));
}

void FbFrameAuthor::OnMasterReplace(wxCommandEvent& event)
{
	FbAuthListData * current = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (current == NULL) return;

	wxString newname;
	int id = FbReplaceDlg::Execute(current->GetCode(), newname);
	if (id == 0) return;

	FbCacheData * cache = new FbCacheData(id, newname);
	FbCollection::AddAuth(cache);

	m_MasterList->Replace(new FbAuthListData(id));
}

void FbFrameAuthor::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbAuthListData * current = wxDynamicCast(model->GetCurrent(), FbAuthListData);
	if (current == NULL) return;

	int id = current->GetCode();

	wxString sql = wxT("SELECT count(id) FROM books WHERE id_author=?");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	int count = result.NextRow() ? result.GetInt(0) : 0;

	wxString msg = _("Delete author") + COLON + current->GetValue(*model);
	if (count) msg << (wxString)wxT("\n") <<  wxString::Format(_("and all of author's books (%d pcs.)?"), count);
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		wxString sql1 = wxString::Format(wxT("DELETE FROM books WHERE id_author=%d"), id);
		wxString sql2 = wxString::Format(wxT("DELETE FROM authors WHERE id=%d"), id);
		(new FbUpdateThread(sql1, sql2))->Execute();
		m_MasterList->Delete();
	}
}

void FbFrameAuthor::OnMasterPage(wxCommandEvent& event)
{
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	if (data && data->GetCode() > 0) {
		wxString host = FbParams::GetText(DB_DOWNLOAD_HOST);
		wxString url = wxString::Format(wxT("http://%s/a/%d"), host.c_str(), data->GetCode());
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
	FbAuthListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbAuthListData);
	event.Enable( data && data->GetCode()>0 );
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
	if (control) event.Check(control->IsShown());
}

void FbFrameAuthor::OnModel( FbArrayEvent& event )
{
	FbAuthListModel * model = new FbAuthListModel(event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameAuthor::OnArray( FbArrayEvent& event )
{
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}

