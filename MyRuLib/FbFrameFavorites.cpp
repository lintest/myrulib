#include "FbFrameFavorites.h"

FbFrameFavorites::FbFrameFavorites(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
    :FbFrameBase(parent, id, title)
{
    CreateControls();
}

void FbFrameFavorites::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	m_Info.Create(this);
	sizer->Add( &m_Info, 1, wxEXPAND, 5 );
	SetSizer(sizer);
	Layout();
}

