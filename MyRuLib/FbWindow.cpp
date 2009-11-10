#include "FbWindow.h"
#include "FbParams.h"

FbDialog::FbDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	:wxDialog( parent, id, title, pos, size, style )
{
	SetFont( FbParams::GetFont(FB_FONT_DLG) );
}

bool FbDialog::Create( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
{
	bool res = wxDialog::Create( parent, id, title, pos, size, style );
	SetFont( FbParams::GetFont(FB_FONT_DLG) );
	return res;
}
