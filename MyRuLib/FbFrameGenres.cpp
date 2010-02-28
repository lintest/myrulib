#include "FbFrameGenres.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbGenres.h"
#include "FbMainMenu.h"

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
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar();
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbMasterList(splitter, ID_MASTER_LIST);
	m_MasterList->AddColumn (_("List of genres"), 100, wxALIGN_LEFT);
	m_MasterList->SetFocus();
	FbGenres::FillControl(m_MasterList);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void FbFrameGenres::Localize(bool bUpdateMenu)
{
	FbFrameBase::Localize(bUpdateMenu);
	m_MasterList->SetColumnText(0, _("Genres"));
	FbGenres::FillControl(m_MasterList);
}

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbMasterData * data = m_MasterList->GetItemData(selected);
		if (data) data->Show(this);
	}
}

void FbFrameGenres::UpdateBooklist()
{
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) {
		FbMasterData * data = m_MasterList->GetItemData(selected);
		if (data) data->Show(this);
	}
}

