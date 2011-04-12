#include "FbFrameDown.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbDownList.h"
#include "MyRuLibApp.h"
#include "res/start.xpm"
#include "res/pause.xpm"

IMPLEMENT_CLASS(FbFrameDown, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameDown, FbFrameBase)
	EVT_MENU(ID_START, FbFrameDown::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameDown::OnPause)
	EVT_MENU(wxID_UP, FbFrameDown::OnMoveUp)
	EVT_MENU(wxID_DOWN, FbFrameDown::OnMoveDown)
	EVT_MENU(ID_DELETE_DOWNLOAD, FbFrameDown::OnSubmenu)
END_EVENT_TABLE()

FbFrameDown::FbFrameDown(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DOWN, GetTitle())
{
	CreateControls();
}

void FbFrameDown::CreateControls()
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	wxPanel * panel = new wxPanel( splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer * bsMasterList = new wxBoxSizer( wxVERTICAL );

	m_ToolBar.Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_NODIVIDER);
	m_ToolBar.SetFont(FbParams::GetFont(FB_FONT_TOOL));
	m_ToolBar.AddTool(ID_START, _("Start"), wxBitmap(start_xpm), _("Start downloading"));
	m_ToolBar.AddTool(ID_PAUSE, _("Stop"), wxBitmap(pause_xpm), _("Stop downloading"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(wxID_UP, _("Up"), wxArtProvider::GetBitmap(wxART_GO_UP), _("Move up in queue"));
	m_ToolBar.AddTool(wxID_DOWN, _("Down"), wxArtProvider::GetBitmap(wxART_GO_DOWN), _("Move down in queue"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(ID_DELETE_DOWNLOAD, _("Delete"), wxArtProvider::GetBitmap(wxART_DELETE), _("Remove download"));
	m_ToolBar.Realize();
	bsMasterList->Add( &m_ToolBar, 0, wxEXPAND, 0 );

	m_MasterList = new FbTreeViewCtrl(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	CreateColumns();
	bsMasterList->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );

	panel->SetSizer( bsMasterList );
	panel->Layout();
	bsMasterList->Fit( panel );

	CreateBooksPanel(splitter);
	splitter->SplitVertically(panel, m_BooksPanel, 160);

	SetSizer( sizer );

	FbFrameBase::CreateControls();
	FillFolders();
}

void FbFrameDown::CreateColumns()
{
	m_MasterList->AddColumn (0, _("Folders"), 100, wxALIGN_LEFT);
}

void FbFrameDown::FillFolders(const int iCurrent)
{
	FbListStore * model = new FbListStore;
	model->Append(new FbDownListData(FbMasterDownInfo::DT_WAIT,  _("Queue")));
	model->Append(new FbDownListData(FbMasterDownInfo::DT_READY, _("Ready")));
	model->Append(new FbDownListData(FbMasterDownInfo::DT_ERROR, _("Fault")));
	m_MasterList->AssignModel(model);
}

void FbFrameDown::UpdateFolder(const int folder, const FbFolderType type)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbDownListData * data = wxDynamicCast(&item, FbDownListData);
	if (data) UpdateBooklist();
}

void FbFrameDown::OnStart(wxCommandEvent & event)
{
	wxGetApp().StartDownload();
}

void FbFrameDown::OnPause(wxCommandEvent & event)
{
	wxGetApp().StopDownload();
}

void FbFrameDown::OnMoveUp(wxCommandEvent& event)
{
/*
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download-1 WHERE download<0 AND md5sum NOT IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD) )->Execute();
*/
}

void FbFrameDown::OnMoveDown(wxCommandEvent& event)
{
/*
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download-1 WHERE download<0 AND md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD))->Execute();
*/
}

