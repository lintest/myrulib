#include "FbFrameDate.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbManager.h"
#include "FbMainMenu.h"
#include "FbCalendar.h"

BEGIN_EVENT_TABLE(FbFrameDate, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDate::OnGenreSelected)
END_EVENT_TABLE()

FbFrameDate::FbFrameDate(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DATE, _("Календарь"))
{
	CreateControls();
}

void FbFrameDate::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(80);
	splitter->SetSashGravity(0);
	bSizer1->Add(splitter, 1, wxEXPAND);

	FbCalendar * calendar = new FbCalendar(splitter);
	m_MasterList = calendar->m_DateList;
	calendar->m_Calendar->SetFocus();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(calendar, m_BooksPanel, 200);

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

FbThreadSkiper FbFrameDate::DateThread::sm_skiper;

void * FbFrameDate::DateThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	if (sm_skiper.Skipped(m_number)) return NULL;
	EmptyBooks();

	wxString condition = wxString::Format(wxT("GENRE(books.genres, ?)"), m_code);
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, wxString::Format(wxT("%02X"), m_code));
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (sm_skiper.Skipped(m_number)) return NULL;
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	return NULL;
}

void FbFrameDate::OnGenreSelected(wxTreeEvent & event)
{
	wxTreeItemId selected = event.GetItem();
	if (selected.IsOk()) {
		m_BooksPanel->EmptyBooks();
//		FbGenreData * data = (FbGenreData*) m_MasterList->GetItemData(selected);
//		if (data) ( new DateThread(this, m_BooksPanel->GetListMode(), data->GetCode()) )->Execute();
	}
}

void FbFrameDate::UpdateBooklist()
{
	int code = 0;
	wxTreeItemId selected = m_MasterList->GetSelection();
	if (selected.IsOk()) {
//		FbGenreData * data = (FbGenreData*) m_MasterList->GetItemData(selected);
//		if (data) code = data->GetCode();
	}
	if (code) ( new DateThread(this, m_BooksPanel->GetListMode(), code) )->Execute();
}

