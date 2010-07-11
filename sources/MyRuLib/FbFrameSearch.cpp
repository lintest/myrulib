#include "FbFrameSearch.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbMainMenu.h"
#include "FbDatabase.h"
#include "FbMasterTypes.h"

//-----------------------------------------------------------------------------
//  FbFrameSearch
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameSearch, FbFrameBase)

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
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_ToolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxID_ANY, GetTitle());
	bSizer1->Add( m_ToolBar, 0, wxGROW);

	CreateBooksPanel(this);
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
	FbMasterInfo info = new FbMasterFindInfo(m_title, m_author);
	m_BooksPanel->Reset(info, m_filter);
}

void FbFrameSearch::OnFoundNothing(wxCommandEvent& event)
{
	wxString msg = wxString::Format(_("Nothing was found on pattern \"%s\""), m_title.c_str());
	wxMessageBox(msg, _("Searching"));
	Close();
}
