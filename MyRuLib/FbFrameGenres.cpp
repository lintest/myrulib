#include "FbFrameGenres.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbGenres.h"
#include "FbMainMenu.h"

FbFrameGenres::SubgenreFunction::SubgenreFunction(int code)
	: m_code( wxString::Format(wxT("%02X"), code) )
{
}

void FbFrameGenres::SubgenreFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(_("SUBGENRE called with wrong number of arguments: %d."), argCount));
		return;
	}
	wxString text = ctx.GetString(0);

	for (size_t i=0; i<text.Length()/2; i++) {
		if ( text.Mid(i*2, 2) == m_code ) {
			ctx.SetResult(true);
			return;
		}
	}
	ctx.SetResult(false);
}

BEGIN_EVENT_TABLE(FbFrameGenres, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameGenres::OnGenreSelected)
END_EVENT_TABLE()

FbFrameGenres::FbFrameGenres(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_GENRES, _("Жанры"))
{
	CreateControls();
}

void FbFrameGenres::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	bSizer1->Add(splitter, 1, wxEXPAND);

	long style = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxSUNKEN_BORDER | wxTR_NO_BUTTONS;
	m_MasterList = new FbTreeListCtrl(splitter, ID_MASTER_LIST, style);
	m_MasterList->AddColumn (_("Список жанров"), 100, wxALIGN_LEFT);
	m_MasterList->SetFocus();
	FbGenres::FillControl(m_MasterList);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

FbThreadSkiper FbFrameGenres::GenresThread::sm_skiper;

void * FbFrameGenres::GenresThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition = wxT("SUBGENRE(books.genres)");
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		database.CreateFunction(wxT("SUBGENRE"), 1, m_subgenre);
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbFrameGenres::OnGenreSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
		FbGenreData * data = (FbGenreData*) m_MasterList->GetItemData(selected);
		if (data) ( new FbFrameGenres::GenresThread(this, m_BooksPanel->GetListMode(), data->GetCode()) )->Execute();
	}
}

void FbFrameGenres::UpdateBooklist()
{
	int code = 0;
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) {
		FbGenreData * data = (FbGenreData*) m_MasterList->GetItemData(selected);
		if (data) code = data->GetCode();
	}
	if (code) ( new FbFrameGenres::GenresThread(this, m_BooksPanel->GetListMode(), code) )->Execute();
}

