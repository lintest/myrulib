#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbMainMenu.h"
#include "FbDatabase.h"
#include "FbManager.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
	EVT_COMMAND(ID_FOUND_NOTHING, fbEVT_BOOK_ACTION, FbFrameSearch::OnFoundNothing)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString & title)
	:FbFrameBase(parent, ID_FRAME_SEARCH, title)
{
	CreateControls();
}

void FbFrameSearch::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( toolbar, 0, wxGROW);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(this, substyle);
	bSizer1->Add( m_BooksPanel, 1, wxEXPAND, 5 );

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void * FbFrameSearch::SearchThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	EmptyBooks();

	wxString condition = wxT("SEARCH_T(books.title)");
	if (!m_author.IsEmpty()) condition += wxT(" AND SEARCH_A(authors.search_name)");
	wxString sql = GetSQL(condition);

	try {
		FbCommonDatabase database;
		InitDatabase(database);
		FbSearchFunction sfTitle(m_title);
		FbSearchFunction sfAuthor(m_author);
		database.CreateFunction(wxT("SEARCH_T"), 1, sfTitle);
		database.CreateFunction(wxT("SEARCH_A"), 1, sfAuthor);
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();

		if (result.Eof()) {
			FbCommandEvent(fbEVT_BOOK_ACTION, ID_FOUND_NOTHING).Post(m_frame);
			return NULL;
		}
		FillBooks(result);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}


	return NULL;
}

void FbFrameSearch::Execute(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author)
{
	if ( title.IsEmpty() ) return;

	wxString msg = wxString::Format(_("Поиск: «%s»"), title.c_str());
	FbFrameSearch * frame = new FbFrameSearch(parent, msg);
	frame->m_author = author;
	frame->m_title = title;
	frame->Update();

	frame->UpdateBooklist();
}

void FbFrameSearch::UpdateBooklist()
{
	( new SearchThread(this, m_BooksPanel->GetListMode(), m_title, m_author) )->Execute();
}

void FbFrameSearch::OnFoundNothing(wxCommandEvent& event)
{
	wxString msg = wxString::Format(_("Ничего не найдено по шаблону «%s»"), m_title.c_str());
	wxMessageBox(msg, wxT("Поиск"));
	Close();
}
