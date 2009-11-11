#include "FbFrameBase.h"
#include "FbConst.h"
#include "FbMainMenu.h"
#include "ExternalDlg.h"
#include "FbMainFrame.h"
#include "InfoCash.h"

BEGIN_EVENT_TABLE(FbFrameBase, wxAuiMDIChildFrame)
	EVT_ACTIVATE(FbFrameBase::OnActivated)
	EVT_TREE_ITEM_COLLAPSING(ID_MASTER_LIST, FbFrameBase::OnTreeCollapsing)
	EVT_MENU(wxID_SAVE, FbFrameBase::OnExternal)
	EVT_MENU(wxID_SELECTALL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_UNSELECTALL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_SPLIT_HORIZONTAL, FbFrameBase::OnChangeView)
	EVT_MENU(ID_SPLIT_VERTICAL, FbFrameBase::OnChangeView)
	EVT_MENU(ID_MODE_TREE, FbFrameBase::OnChangeMode)
	EVT_MENU(ID_MODE_LIST, FbFrameBase::OnChangeMode)
	EVT_MENU(ID_FILTER_FB2, FbFrameBase::OnChangeFilter)
	EVT_MENU(ID_FILTER_LIB, FbFrameBase::OnChangeFilter)
	EVT_MENU(ID_FILTER_USR, FbFrameBase::OnChangeFilter)
	EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbFrameBase::OnChangeViewUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbFrameBase::OnChangeViewUpdateUI)
	EVT_UPDATE_UI(ID_MODE_LIST, FbFrameBase::OnChangeModeUpdateUI)
	EVT_UPDATE_UI(ID_MODE_TREE, FbFrameBase::OnChangeModeUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_FB2, FbFrameBase::OnChangeFilterUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_LIB, FbFrameBase::OnChangeFilterUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_USR, FbFrameBase::OnChangeFilterUpdateUI)
	EVT_COMMAND(ID_EMPTY_BOOKS, fbEVT_BOOK_ACTION, FbFrameBase::OnEmptyBooks)
	EVT_COMMAND(ID_APPEND_AUTHOR, fbEVT_BOOK_ACTION, FbFrameBase::OnAppendAuthor)
	EVT_COMMAND(ID_APPEND_SEQUENCE, fbEVT_BOOK_ACTION, FbFrameBase::OnAppendSequence)
	EVT_FB_BOOK(ID_APPEND_BOOK, FbFrameBase::OnAppendBook)
END_EVENT_TABLE()

FbFrameBase::FbFrameBase() :
	m_FilterFb2(FbParams::GetValue(FB_FILTER_FB2)),
	m_FilterLib(FbParams::GetValue(FB_FILTER_LIB)),
	m_FilterUsr(FbParams::GetValue(FB_FILTER_USR)),
	m_MasterList(NULL)
{
}

FbFrameBase::FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title) :
	m_FilterFb2(FbParams::GetValue(FB_FILTER_FB2)),
	m_FilterLib(FbParams::GetValue(FB_FILTER_LIB)),
	m_FilterUsr(FbParams::GetValue(FB_FILTER_USR)),
	m_MasterList(NULL)
{
	Create(parent, id, title);
}

bool FbFrameBase::Create(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if (res) {
		SetMenuBar(new FbFrameMenu);
		CreateControls();
	}
	return res;
}

void FbFrameBase::CreateBooksPanel(wxWindow * parent, long substyle)
{
	m_BooksPanel = new FbBookPanel(parent, wxSize(500, 400), substyle, GetViewKey(), GetModeKey());
}

void FbFrameBase::OnSubmenu(wxCommandEvent& event)
{
	wxPostEvent(m_BooksPanel, event);
}

void FbFrameBase::OnChangeViewUpdateUI(wxUpdateUIEvent & event)
{
	if (event.GetId() == ID_SPLIT_HORIZONTAL && m_BooksPanel->GetSplitMode() == wxSPLIT_HORIZONTAL) event.Check(true);
	if (event.GetId() == ID_SPLIT_VERTICAL && m_BooksPanel->GetSplitMode() == wxSPLIT_VERTICAL) event.Check(true);
}

void FbFrameBase::OnChangeModeUpdateUI(wxUpdateUIEvent & event)
{
	if (event.GetId() == ID_MODE_LIST && m_BooksPanel->GetListMode() == FB2_MODE_LIST) event.Check(true);
	if (event.GetId() == ID_MODE_TREE && m_BooksPanel->GetListMode() == FB2_MODE_TREE) event.Check(true);
}

void FbFrameBase::OnExternal(wxCommandEvent& event)
{
	ExternalDlg::Execute(this, m_BooksPanel->m_BookList);
}

void FbFrameBase::OnEmptyBooks(wxCommandEvent& event)
{
	m_BooksPanel->EmptyBooks();
}

void FbFrameBase::OnAppendBook(FbBookEvent& event)
{
	m_BooksPanel->AppendBook( new BookTreeItemData(event.m_data), event.GetString() );
}

void FbFrameBase::OnAppendAuthor(wxCommandEvent& event)
{
	m_BooksPanel->AppendAuthor( event.GetString() );
}

void FbFrameBase::OnAppendSequence(wxCommandEvent& event)
{
	m_BooksPanel->AppendSequence( event.GetString() );
}

void FbFrameBase::OnChangeFilterUpdateUI(wxUpdateUIEvent & event)
{
	switch (event.GetId()) {
		case ID_FILTER_FB2: event.Check(m_FilterFb2); break;
		case ID_FILTER_LIB: event.Check(m_FilterLib); break;
		case ID_FILTER_USR: event.Check(m_FilterUsr); break;
	}
}

int FbFrameBase::GetModeKey()
{
	switch (GetId()) {
		case ID_FRAME_AUTHOR: return FB_MODE_AUTHOR;
		case ID_FRAME_GENRES: return FB_MODE_GENRES;
		case ID_FRAME_FOLDER: return FB_MODE_FOLDER;
		case ID_FRAME_SEARCH: return FB_MODE_SEARCH;
		default: return 0;
	}
}

int FbFrameBase::GetViewKey()
{
	switch (GetId()) {
		case ID_FRAME_AUTHOR: return FB_VIEW_AUTHOR;
		case ID_FRAME_GENRES: return FB_VIEW_GENRES;
		case ID_FRAME_FOLDER: return FB_VIEW_FOLDER;
		case ID_FRAME_SEARCH: return FB_VIEW_SEARCH;
		case ID_FRAME_DOWNLD: return FB_VIEW_DOWNLD;
		default: return 0;
	}
}

void FbFrameBase::OnChangeView(wxCommandEvent & event)
{
	int vertical = (event.GetId() == ID_SPLIT_VERTICAL);
	m_BooksPanel->CreateBookInfo((bool)vertical);

	int param = GetViewKey();
	if (param) FbParams().SetValue(param, vertical);
}

void FbFrameBase::OnChangeMode(wxCommandEvent& event)
{
	FbListMode mode = event.GetId() == ID_MODE_TREE ? FB2_MODE_TREE : FB2_MODE_LIST;
	m_BooksPanel->CreateColumns(mode);
	UpdateBooklist();

	int param = GetModeKey();
	if (param) FbParams().SetValue(param, mode);
}

void FbFrameBase::OnChangeFilter(wxCommandEvent& event)
{
	FbParams params;
	switch (event.GetId()) {
		case ID_FILTER_FB2: {
			params.SetValue(FB_FILTER_FB2, m_FilterFb2 = !m_FilterFb2);
		} break;
		case ID_FILTER_LIB: {
			params.SetValue(FB_FILTER_LIB, m_FilterLib = !m_FilterLib);
			if (m_FilterLib) {
				params.SetValue(FB_FILTER_USR, false);
				m_FilterUsr = false;
			}
		} break;
		case ID_FILTER_USR: {
			params.SetValue(FB_FILTER_USR, m_FilterUsr = !m_FilterUsr);
			if (m_FilterUsr) {
				params.SetValue(FB_FILTER_LIB, false);
				m_FilterLib = false;
			}
		} break;
	}
	UpdateBooklist();
}

void FbFrameBase::OnTreeCollapsing(wxTreeEvent & event)
{
	event.Veto();
}

void FbFrameBase::OnActivated(wxActivateEvent & event)
{
	FbMainFrame * frame = wxDynamicCast(GetMDIParentFrame(), FbMainFrame);
	if (frame) {

	}
}

void FbFrameBase::UpdateFonts(bool refresh)
{
	if (m_MasterList) {
		m_MasterList->SetFont( FbParams::GetFont(FB_FONT_MAIN) );
		if (refresh) m_MasterList->Update();
	}
	m_BooksPanel->UpdateFonts(refresh);
}

void FbFrameBase::UpdateInfo(int id)
{
	InfoCash::UpdateInfo(m_BooksPanel, id, m_BooksPanel->GetSplitMode() == wxSPLIT_VERTICAL);
}
