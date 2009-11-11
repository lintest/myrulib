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

bool FbAuiMDIChildFrame::Create(wxAuiMDIParentFrame *parent, wxWindowID winid, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	return wxAuiMDIChildFrame::Create(parent, winid, title, pos, size, style, name);
}

void FbAuiMDIChildFrame::UpdateFont(wxHtmlWindow * html, bool refresh)
{
	wxFont font = FbParams::GetFont(FB_FONT_HTML);

	int fontsizes[7] = {6, 8, 9, 10, 12, 16, 18};
	int size = font.GetPointSize();

	fontsizes[0] = size - 2;
	fontsizes[1] = size;
	fontsizes[2] = size + 1;
	fontsizes[3] = size + 2;
	fontsizes[4] = size + 4;
	fontsizes[5] = size + 8;
	fontsizes[6] = size + 10;

	html->SetFonts(font.GetFaceName(), font.GetFaceName(), fontsizes);
}


