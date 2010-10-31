#include "FbFrameFind.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbMainMenu.h"
#include "FbDatabase.h"
#include "FbMasterTypes.h"

//-----------------------------------------------------------------------------
//  FbFrameFind
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameFind, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameFind, FbFrameBase)
	EVT_COMMAND(ID_FOUND_NOTHING, fbEVT_BOOK_ACTION, FbFrameFind::OnFoundNothing)
	EVT_COMMAND(ID_INIT_SEARCH, fbEVT_BOOK_ACTION, FbFrameFind::OnInitSearch)
END_EVENT_TABLE()

FbFrameFind::FbFrameFind(wxAuiMDIParentFrame * parent, const FbMasterInfo &info, const wxString &title)
	:FbFrameBase(parent, ID_FRAME_FIND, wxEmptyString), m_info(info)
{
	SetTitle(title);
	CreateControls();

	if (!info.IsKindOf(CLASSINFO(FbMasterFindInfo))) {
		m_BooksPanel->SetListMode(FB2_MODE_TREE);
	}

	Update();

	FbCommandEvent(fbEVT_BOOK_ACTION, ID_INIT_SEARCH).Post(this);
}
/*
wxString FbFrameFind::GetTitle() const
{
	return _("Search") + COLON + m_title;
}
*/
void FbFrameFind::CreateControls()
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	CreateBooksPanel(this);
	bSizer1->Add( m_BooksPanel, 1, wxEXPAND, 5 );

	SetSizer( bSizer1 );

	FbFrameBase::CreateControls();
}

void FbFrameFind::OnFoundNothing(wxCommandEvent& event)
{
	wxString msg = wxString::Format(_("Nothing was found on pattern \"%s\""), m_title.c_str());
	wxMessageBox(msg, _("Searching"));
	Close();
}

void FbFrameFind::OnInitSearch(wxCommandEvent& event)
{
	UpdateBooklist();
}

