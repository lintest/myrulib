#include "FbFrameDownld.h"
#include <wx/artprov.h>
#include "FbBookMenu.h"
#include "FbMainMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbDownloader.h"
#include "FbUpdateThread.h"
#include "res/start.xpm"
#include "res/pause.xpm"

IMPLEMENT_CLASS(FbDownListData, FbModelData)

IMPLEMENT_CLASS(FbFrameDownld, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameDownld, FbFrameBase)
	EVT_MENU(ID_START, FbFrameDownld::OnStart)
	EVT_MENU(ID_PAUSE, FbFrameDownld::OnPause)
	EVT_MENU(wxID_UP, FbFrameDownld::OnMoveUp)
	EVT_MENU(wxID_DOWN, FbFrameDownld::OnMoveDown)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDownld::OnFolderSelected)
	EVT_MENU(ID_DELETE_DOWNLOAD, FbFrameDownld::OnSubmenu)
END_EVENT_TABLE()

FbFrameDownld::FbFrameDownld(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DOWNLD, GetTitle())
{
	CreateControls();
}

void FbFrameDownld::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar();
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbTreeViewCtrl(splitter, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	CreateColumns();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FillFolders();

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void FbFrameDownld::Localize(bool bUpdateMenu)
{
	FbFrameBase::Localize(bUpdateMenu);
	m_MasterList->EmptyColumns();
	CreateColumns();
	m_MasterList->Refresh();
}

void FbFrameDownld::CreateColumns()
{
	m_MasterList->AddColumn (0, _("Folders"), 100, wxALIGN_LEFT);
}

wxToolBar * FbFrameDownld::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(FbParams::GetFont(FB_FONT_TOOL));

	toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Export to external device"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_START, _("Start"), wxBitmap(start_xpm), _("Start downloading"));
	toolbar->AddTool(ID_PAUSE, _("Stop"), wxBitmap(pause_xpm), _("Stop downloading"));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_UP, _("Up"), wxArtProvider::GetBitmap(wxART_GO_UP), _("Move up in queue"));
	toolbar->AddTool(wxID_DOWN, _("Down"), wxArtProvider::GetBitmap(wxART_GO_DOWN), _("Move down in queue"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_DELETE_DOWNLOAD, _("Delete"), wxArtProvider::GetBitmap(wxART_DELETE), _("Remove download"));
	toolbar->Realize();

	return toolbar;
}

void FbFrameDownld::FillFolders(const int iCurrent)
{
	FbListStore * model = new FbListStore;
	model->Append(new FbDownListData(+1, _("Queue")));
	model->Append(new FbDownListData(-1, _("Ready")));
	model->Append(new FbDownListData(-2, _("Fault")));
	m_MasterList->AssignModel(model);
}

void FbFrameDownld::OnFolderSelected(wxTreeEvent & event)
{
	FbDownListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbDownListData);
	if (data) FbMasterDownld(data->GetCode()).Show(this);
}

void FbFrameDownld::UpdateBooklist()
{
	FbDownListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbDownListData);
	if (data) FbMasterDownld(data->GetCode()).Show(this);
}

void FbFrameDownld::UpdateFolder(const int folder, const FbFolderType type)
{
	FbDownListData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbDownListData);
	if (data && data->GetCode() == folder) FbMasterDownld(data->GetCode()).Show(this);
}

void FbFrameDownld::OnStart(wxCommandEvent & event)
{
	FbDownloader::Start();
}

void FbFrameDownld::OnPause(wxCommandEvent & event)
{
	FbDownloader::Pause();
}

void FbFrameDownld::OnMoveUp(wxCommandEvent& event)
{
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download+1 WHERE download>0 AND md5sum NOT IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD) )->Execute();
}

void FbFrameDownld::OnMoveDown(wxCommandEvent& event)
{
	wxString sel = m_BooksPanel->m_BookList->GetSelected();
	if (sel.IsEmpty()) return;

	wxString sql = wxString::Format(wxT("\
		UPDATE states SET download=download+1 WHERE download>0 AND md5sum IN \
		(SELECT DISTINCT md5sum FROM books WHERE id IN (%s)) \
	"), sel.c_str());

	( new FbFolderUpdateThread(sql, 1, FT_DOWNLOAD))->Execute();
}

