#include "FbFrameFavorites.h"

FbFrameFavorites::FbFrameFavorites(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameFavorites::CreateControls()
{
	SetSizer(new wxBoxSizer(wxVERTICAL));
	SetMenuBar(CreateMenuBar());
}
