#include "FbFrameFind.h"
#include <wx/artprov.h>
#include <wx/mimetype.h>
#include "FbConst.h"
#include "FbBookPanel.h"
#include "FbMainMenu.h"
#include "FbDatabase.h"
#include "FbMasterTypes.h"

//-----------------------------------------------------------------------------
//  FbFrameFind
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameFind, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameFind, FbFrameBase)
	EVT_COMMAND(ID_INIT_SEARCH, fbEVT_BOOK_ACTION, FbFrameFind::OnInitSearch)
END_EVENT_TABLE()

FbFrameFind::FbFrameFind(wxAuiNotebook * parent, wxWindowID winid, const FbMasterInfo &info, const wxString &title)
	: FbFrameBase(parent, winid, title, true)
	, m_info(info)
	, m_title(title)
{
	CreateBooksPanel(this);
	Initialize(m_BooksPanel);
	CreateControls(true);

	Update();

	FbCommandEvent(fbEVT_BOOK_ACTION, ID_INIT_SEARCH).Post(this);
}

void FbFrameFind::OnInitSearch(wxCommandEvent& event)
{
	UpdateBooklist();
}

