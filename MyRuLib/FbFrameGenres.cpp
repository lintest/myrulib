#include "FbFrameGenres.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbGenres.h"
#include "FbMainMenu.h"

IMPLEMENT_CLASS(FbFrameGenres, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameGenres, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameGenres::OnGenreSelected)
END_EVENT_TABLE()

FbFrameGenres::FbFrameGenres(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_GENRES, GetTitle())
{
	CreateControls();
}

void FbFrameGenres::CreateControls()
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

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
	CreateColumns();
}

void FbFrameGenres::Localize(bool bUpdateMenu)
{
	m_MasterList->EmptyColumns();
	CreateColumns();
	FbFrameBase::Localize(bUpdateMenu);
}

void FbFrameGenres::CreateColumns()
{
	m_MasterList->AddColumn (0, _("List of genres"), 100, wxALIGN_LEFT);
	FbModel * model = FbGenres::CreateModel();
	m_MasterList->AssignModel(model);
}

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	m_BooksPanel->EmptyBooks();
	FbModelItem item = m_MasterList->GetCurrent();
	FbGenreChildData * data = wxDynamicCast(&item, FbGenreChildData);
	if (data) FbMasterGenre(data->GetCode()).Show(this);
	else if (m_BooksPanel) m_BooksPanel->AssignEmpty();
}

void FbFrameGenres::UpdateBooklist()
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbGenreChildData * data = wxDynamicCast(&item, FbGenreChildData);
	if (data) FbMasterGenre(data->GetCode()).Show(this);
	else if (m_BooksPanel) m_BooksPanel->AssignEmpty();
}

