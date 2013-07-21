#include "FbFrameDown.h"
#include <wx/artprov.h>
#include "FbBookPanel.h"
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "models/FbDownList.h"
#include "MyRuLibApp.h"

IMPLEMENT_CLASS(FbFrameDown, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameDown, FbFrameBase)
	EVT_MENU(ID_START, FbFrameDown::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameDown::OnPause)
	EVT_MENU(wxID_UP, FbFrameDown::OnMoveUp)
	EVT_MENU(wxID_DOWN, FbFrameDown::OnMoveDown)
	EVT_MENU(ID_DELETE_DOWNLOAD, FbFrameDown::OnSubmenu)
END_EVENT_TABLE()

#ifdef __WXGTK__
	#define fbART_START wxT("gtk-media-play")
	#define fbART_PAUSE wxT("gtk-media-pause")
#else
	#include "res/start.xpm"
	#include "res/pause.xpm"
	#define fbART_START wxBitmap(start_xpm)
	#define fbART_PAUSE wxBitmap(pause_xpm)
#endif

FbFrameDown::FbFrameDown(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_DOWN, _("Downloads"), select)
{
	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_ToolBar.Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_NODIVIDER);
	m_ToolBar.SetFont(FbParams(FB_FONT_TOOL));
	m_ToolBar.AddTool(ID_START, _("Start"), fbART_START, _("Start downloading"));
	m_ToolBar.AddTool(ID_PAUSE, _("Stop"), fbART_PAUSE, _("Stop downloading"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(wxID_UP, _("Up"), wxART_GO_UP, _("Move up in queue"));
	m_ToolBar.AddTool(wxID_DOWN, _("Down"), wxART_GO_DOWN, _("Move down in queue"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(ID_DELETE_DOWNLOAD, _("Delete"), wxART_DELETE, _("Remove download"));
	m_ToolBar.Realize();
	sizer->Add( &m_ToolBar, 0, wxEXPAND, 0 );

	m_MasterList = new FbMasterViewCtrl;
	m_MasterList->Create(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, FbParams.Style());
	CreateColumns();
	sizer->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );

	panel->SetSizer( sizer );
	panel->Layout();
	sizer->Fit( panel );

	CreateBooksPanel(this);
	SplitVertically(panel, m_BooksPanel);

	CreateControls(select);
	FillFolders();
}

void FbFrameDown::CreateColumns()
{
	m_MasterList->AddColumn (0, _("Folders"), -10, wxALIGN_LEFT);
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

void FbFrameDown::UpdateBooklist()
{
	FbTreeViewCtrl & booklist = m_BooksPanel->GetBookList();
	FbMasterDownInfo info = FbMasterDownInfo::DT_WAIT;
	if (GetInfo() == info) {
		booklist.SetSortedColumn(0);
		m_ToolBar.EnableTool(wxID_UP, true);
		m_ToolBar.EnableTool(wxID_DOWN, true);
	} else {
		m_ToolBar.EnableTool(wxID_UP, false);
		m_ToolBar.EnableTool(wxID_DOWN, false);
		if (booklist.GetSortedColumn() == 0) {
			int order = m_BooksPanel->GetListMode() == FB2_MODE_TREE ? BF_NUMB : BF_NAME;
			booklist.SetSortedColumn(order + 1);
		}
	}
	FbFrameBase::UpdateBooklist();
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
	wxString sel = m_BooksPanel->m_BookList.GetSelected();
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
	wxString sel = m_BooksPanel->m_BookList.GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download-1 WHERE download<0 AND md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD))->Execute();
*/
}

