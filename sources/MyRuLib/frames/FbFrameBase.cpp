#include "FbFrameBase.h"
#include <wx/clipbrd.h>
#include "FbConst.h"
#include "FbBookPanel.h"
#include "FbFrameThread.h"
#include "FbMainFrame.h"
#include "MyRuLibApp.h"
#include "FbMasterTypes.h"
#include "dialogs/FbExportDlg.h"
#include "dialogs/FbFilterDlg.h"
#include "dialogs/FbColumnDlg.h"

//-----------------------------------------------------------------------------
//  FbMasterViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbMasterViewCtrl, FbTreeViewCtrl)

BEGIN_EVENT_TABLE(FbMasterViewCtrl, FbTreeViewCtrl)
	EVT_MENU(wxID_COPY, FbMasterViewCtrl::OnCopy)
	EVT_MENU(wxID_SELECTALL, FbMasterViewCtrl::OnSelect)
	EVT_MENU(ID_UNSELECTALL, FbMasterViewCtrl::OnSelect)
	EVT_UPDATE_UI(wxID_CUT, FbMasterViewCtrl::OnDisableUI)
	EVT_UPDATE_UI(wxID_COPY, FbMasterViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(wxID_PASTE, FbMasterViewCtrl::OnDisableUI)
	EVT_UPDATE_UI(wxID_SELECTALL, FbMasterViewCtrl::OnDisableUI)
	EVT_UPDATE_UI(ID_UNSELECTALL, FbMasterViewCtrl::OnDisableUI)
END_EVENT_TABLE()

void FbMasterViewCtrl::OnCopy(wxCommandEvent& event)
{
	wxString text = this->GetCurrentText();
	if (text.IsEmpty()) return;

	wxClipboardLocker locker;
	if (!locker) return;
	wxTheClipboard->SetData( new wxTextDataObject(text) );
}

//-----------------------------------------------------------------------------
//  FbFrameBase
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(FbFrameBase, wxSplitterWindow)

BEGIN_EVENT_TABLE(FbFrameBase, wxSplitterWindow)
	EVT_COMMAND(ID_AUTHOR_INFO, fbEVT_BOOK_ACTION, FbFrameBase::OnSubmenu)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameBase::OnMasterSelected)
	EVT_MENU(wxID_ANY, FbFrameBase::OnHandleMenu)
	EVT_MENU(wxID_SAVE, FbFrameBase::OnExportBooks)
	EVT_MENU(ID_SPLIT_HORIZONTAL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_SPLIT_VERTICAL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_SPLIT_NOTHING, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_EDIT_COMMENTS, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_MODE_TREE, FbFrameBase::OnChangeMode)
	EVT_MENU(ID_MODE_LIST, FbFrameBase::OnChangeMode)
	EVT_MENU(ID_FILTER_SET, FbFrameBase::OnFilterSet)
	EVT_MENU(ID_FILTER_USE, FbFrameBase::OnFilterUse)
	EVT_MENU(ID_FILTER_DEL, FbFrameBase::OnFilterDel)
	EVT_MENU(ID_DIRECTION, FbFrameBase::OnDirection)
	EVT_MENU(wxID_VIEW_SORTNAME, FbFrameBase::OnChangeOrder)
	EVT_MENU(wxID_VIEW_SORTDATE, FbFrameBase::OnChangeOrder)
	EVT_MENU(wxID_VIEW_SORTSIZE, FbFrameBase::OnChangeOrder)
	EVT_MENU(wxID_VIEW_SORTTYPE, FbFrameBase::OnChangeOrder)
	EVT_MENU(ID_ORDER_AUTHOR, FbFrameBase::OnChangeOrder)
	EVT_MENU(ID_ORDER_RATING, FbFrameBase::OnChangeOrder)
	EVT_MENU(ID_ORDER_LANG, FbFrameBase::OnChangeOrder)
	EVT_MENU(ID_SHOW_COLUMNS, FbFrameBase::OnShowColumns)
	EVT_UPDATE_UI(ID_MODE_LIST, FbFrameBase::OnChangeModeUpdateUI)
	EVT_UPDATE_UI(ID_MODE_TREE, FbFrameBase::OnChangeModeUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbFrameBase::OnChangeViewUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbFrameBase::OnChangeViewUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_NOTHING, FbFrameBase::OnChangeViewUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_USE, FbFrameBase::OnFilterUseUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_DEL, FbFrameBase::OnFilterDelUpdateUI)
	EVT_UPDATE_UI(ID_DIRECTION, FbFrameBase::OnDirectionUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTNAME, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTDATE, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTSIZE, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTTYPE, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_AUTHOR, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_LANG, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_RATING, FbFrameBase::OnChangeOrderUpdateUI)
	EVT_LIST_COL_CLICK(ID_BOOKLIST_CTRL, FbFrameBase::OnColClick)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameBase::OnBooksCount)
END_EVENT_TABLE()

FbFrameBase::FbFrameBase(wxAuiNotebook * parent, wxWindowID winid, const wxString & caption, bool select)
	: wxSplitterWindow(parent, winid, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER | wxTAB_TRAVERSAL)
	, m_MasterList(NULL)
	, m_BooksPanel(NULL)
	, m_MasterThread(NULL)
	, m_CountThread(NULL)
	, m_BookCount(0)
{
	parent->AddPage( this, caption, select );
	Connect( wxEVT_IDLE, wxIdleEventHandler( FbFrameBase::OnIdleSplitter ), NULL, this );
	m_filter.Load();
}

FbFrameBase::~FbFrameBase()
{
	if (m_MasterThread) {
		m_MasterThread->Close();
		m_MasterThread->Wait();
		wxDELETE(m_MasterThread);
	}
	if (m_CountThread) {
		m_CountThread->Close();
		m_CountThread->Wait();
		wxDELETE(m_CountThread);
	}
	if (!m_MasterFile.IsEmpty()) {
		if (m_MasterList) m_MasterList->AssignModel(NULL);
		wxRemoveFile(m_MasterFile);
	}
}

void FbFrameBase::CreateControls(bool select)
{
	UpdateFonts(false);
	ShowFullScreen(IsFullScreen());
	Layout();

	if (select) {
		if (m_MasterList) {
			m_MasterList->SetFocus();
		} else {
			m_BooksPanel->SetFocus();
		}
	}

	SetMinimumPaneSize(50);
}

void FbFrameBase::Localize(bool bUpdateMenu)
{
	if (m_MasterList) {
		m_MasterList->EmptyColumns();
		CreateColumns();
		m_MasterList->Refresh();
	}
	m_BooksPanel->Localize();
}

void FbFrameBase::CreateBooksPanel(wxWindow * parent)
{
	m_BooksPanel = new FbBookPanel(parent, wxSize(500, 400), GetId());
}

FbListMode FbFrameBase::GetListMode()
{
	return m_BooksPanel->GetListMode();
}

void FbFrameBase::RefreshBooks()
{
	m_BooksPanel->GetBookList().Refresh();
}

void FbFrameBase::OnSubmenu(wxCommandEvent& event)
{
	m_BooksPanel->DoEvent(event);
}

void FbFrameBase::OnExportBooks(wxCommandEvent& event)
{
	FbMasterInfo info = GetInfo();
	FbMasterAuthInfo * auth = wxDynamicCast(&info, FbMasterAuthInfo);
	FbExportDlg::Execute(this, m_BooksPanel, auth ? auth->GetId() : 0);
}

void FbFrameBase::OnMasterSelected(wxTreeEvent & event)
{
	UpdateBooklist();
}

void FbFrameBase::OnChangeOrder(wxCommandEvent& event)
{
	m_BooksPanel->SetOrderID( event.GetId() );
	UpdateBooklist();
}

void FbFrameBase::OnDirection(wxCommandEvent& event)
{
	m_BooksPanel->RevertOrder();
	UpdateBooklist();
}

void FbFrameBase::OnChangeMode(wxCommandEvent& event)
{
	FbListMode listmode = event.GetId() == ID_MODE_TREE ? FB2_MODE_TREE : FB2_MODE_LIST;
	FbParams(GetId(), FB_LIST_MODE) = listmode;
	m_BooksPanel->SetListMode(listmode);
	UpdateBooklist();
}

void FbFrameBase::UpdateFonts(bool refresh)
{
	if (m_MasterList) {
		m_MasterList->SetFont( FbParams(FB_FONT_MAIN) );
		if (refresh) m_MasterList->Update();
	}
	if (m_BooksPanel) {
		m_BooksPanel->UpdateFonts(refresh);
	}
}

void FbFrameBase::UpdateInfo(int id)
{
	if (m_BooksPanel) m_BooksPanel->UpdateInfo(id);
}

void FbFrameBase::OnDirectionUpdateUI(wxUpdateUIEvent & event)
{
	event.Check( m_BooksPanel->GetBookList().GetSortedColumn() < 0 );
}

void FbFrameBase::OnChangeOrderUpdateUI(wxUpdateUIEvent & event)
{
	if (event.GetId() == m_BooksPanel->GetOrderID()) event.Check(true);
}

void FbFrameBase::OnColClick(wxListEvent& event)
{
	UpdateBooklist();
}

void FbFrameBase::OnBooksCount(FbCountEvent& event)
{
	if (event.GetInfo() == GetInfo()) {
		m_BookCount = event.GetCount();
	}
}

void FbFrameBase::ShowFullScreen(bool show)
{
	Layout();
}

bool FbFrameBase::IsFullScreen()
{
	wxTopLevelWindow * frame = (wxTopLevelWindow*) wxGetApp().GetTopWindow();
	return frame->IsFullScreen();
}

void FbFrameBase::OnFilterUseUpdateUI(wxUpdateUIEvent & event)
{
	event.Check(m_filter.IsEnabled());
}

void FbFrameBase::OnFilterDelUpdateUI(wxUpdateUIEvent & event)
{
	event.Check(m_filter.GetDeleted());
}

void FbFrameBase::OnFilterSet(wxCommandEvent& event)
{
	if (FbFilterDlg::Execute(m_filter)) {
		UpdateBooklist();
		UpdateCounter();
	}
}

void FbFrameBase::OnFilterUse(wxCommandEvent& event)
{
	bool use = !m_filter.IsEnabled();
	FbParams(FB_USE_FILTER) = use;
	m_filter.Enable(use);
	UpdateBooklist();
	UpdateCounter();
}

void FbFrameBase::OnFilterDel(wxCommandEvent& event)
{
	bool del = !m_filter.GetDeleted();
	FbParams(FB_FILTER_DEL) = del;
	m_filter.SetDeleted(del);
	UpdateBooklist();
	UpdateCounter();
}

void FbFrameBase::OnChangeModeUpdateUI(wxUpdateUIEvent & event)
{
	FbListMode mode = m_BooksPanel->GetListMode();
	switch (event.GetId()) {
		case ID_MODE_LIST: if (mode == FB2_MODE_LIST) event.Check(true); break;
		case ID_MODE_TREE: if (mode == FB2_MODE_TREE) event.Check(true); break;
	}
}

void FbFrameBase::OnChangeViewUpdateUI(wxUpdateUIEvent & event)
{
	FbViewMode mode = m_BooksPanel->GetViewMode();
	switch (event.GetId()) {
		case ID_SPLIT_HORIZONTAL: if (mode == FB2_VIEW_HORISONTAL) event.Check(true); break;
		case ID_SPLIT_VERTICAL: if (mode == FB2_VIEW_VERTICAL) event.Check(true); break;
		case ID_SPLIT_NOTHING: if (mode == FB2_VIEW_NOTHING) event.Check(true); break;
	}
}

void FbFrameBase::OnShowColumns(wxCommandEvent& event)
{
	wxArrayInt columns;
	m_BooksPanel->GetBookList().GetColumns(columns);
	bool ok = FbColumnDlg::Execute(this, columns);
	if (ok) {
		m_BooksPanel->CreateColumns(columns);
		wxString text = FbColumns::Get(columns);
		FbParams(GetId(), FB_BOOK_COLUMNS) = text;
	}
}

void FbFrameBase::UpdateBooklist()
{
	m_BooksPanel->Reset(GetInfo(), m_filter);
}

void FbFrameBase::UpdateCounter()
{
	FbFrameThread * thread = CreateCounter();
	if (!thread) return;

	if (m_MasterThread) {
		m_MasterThread->Wait();
		wxDELETE(m_MasterThread);
	}

	if (m_CountThread) {
		m_CountThread->Close();
		m_CountThread->Wait();
		wxDELETE(m_CountThread);
	}

	(m_CountThread = thread)->Execute();
}

FbFrameThread * FbFrameBase::CreateCounter()
{
	wxString sql = GetCountSQL();
	if (sql.IsEmpty()) return NULL;
	FbCountThread * thread = new FbCountThread(this);
	thread->SetCountSQL(sql, m_filter);
	return thread;
}

FbMasterInfo FbFrameBase::GetInfo()
{
	return m_MasterList ? m_MasterList->GetInfo() : FbMasterInfo();
}

void FbFrameBase::OnHandleMenu(wxCommandEvent& event)
{
	if (event.GetId() > ID_FAVORITES_ADD) {
		m_BooksPanel->DoPopupMenu(event.GetId());
	} else event.Skip();
}

void FbFrameBase::UpdateMaster(FbMasterEvent & event)
{
	if (m_BooksPanel && GetInfo() == event.m_info) m_BooksPanel->UpdateMaster(event);
}

void FbFrameBase::OnIdleSplitter( wxIdleEvent& )
{
	Disconnect( wxEVT_IDLE, wxIdleEventHandler( FbFrameBase::OnIdleSplitter ), NULL, this );
	SetSashPosition( GetWindowSize() / 3 );
	SetSashGravity( 0.333 );
	m_lastSize = GetSize();
}

void FbFrameBase::UpdateMaster()
{
	UpdateCounter();
}
