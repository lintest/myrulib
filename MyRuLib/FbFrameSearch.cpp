#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbMainMenu.h"
#include "FbDatabase.h"

BEGIN_EVENT_TABLE(FbFrameSearch, FbFrameBase)
	EVT_COMMAND(ID_FOUND_NOTHING, fbEVT_BOOK_ACTION, FbFrameSearch::OnFoundNothing)
END_EVENT_TABLE()

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author)
	:FbFrameBase(parent, ID_FRAME_SEARCH, wxEmptyString), m_title(title), m_author(author)
{
	CreateControls();
	SetTitle(GetTitle());
}

wxString FbFrameSearch::GetTitle() const
{
	return _("Search") + COLON + m_title;
}

void FbFrameSearch::CreateControls()
{
	SetMenuBar(new FbFrameMenu);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(this, substyle);
	bSizer1->Add( m_BooksPanel, 1, wxEXPAND, 5 );

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void FbFrameSearch::Execute(wxAuiMDIParentFrame * parent, const wxString &title, const wxString &author)
{
	if ( title.IsEmpty() ) return;

	wxString msg = _("Searching") + COLON + title;
	FbFrameSearch * frame = new FbFrameSearch(parent, title, author);
	frame->Update();

	frame->UpdateBooklist();
}

void FbFrameSearch::UpdateBooklist()
{
	FbMasterSearch(m_title, m_author).Show(this);
}

void FbFrameSearch::OnFoundNothing(wxCommandEvent& event)
{
	wxString msg = wxString::Format(_("Nothing was found on pattern \"%s\""), m_title.c_str());
	wxMessageBox(msg, _("Searching"));
	Close();
}
