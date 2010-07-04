#include "FbWindow.h"
#include "FbParams.h"
#include "FbMainMenu.h"

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

FbDialog::FbDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
	:wxDialog( parent, id, title, pos, size, style, name )
{
	SetFont( FbParams::GetFont(FB_FONT_DLG) );
}

bool FbDialog::Create( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
	bool res = wxDialog::Create( parent, id, title, pos, size, style, name );
	SetFont( FbParams::GetFont(FB_FONT_DLG) );
	return res;
}

//-----------------------------------------------------------------------------
//  FbAuiMDIParentFrame
//-----------------------------------------------------------------------------

FbAuiMDIParentFrame::~FbAuiMDIParentFrame()
{ 
	if (m_menubar) DetachMenuBar();
	wxDELETE(m_menubar); 
}

void FbAuiMDIParentFrame::SetMainMenu(wxMenuBar * menubar)
{
	if (m_menubar) {
	    DetachMenuBar();
		delete m_menubar;
	}

    if (menubar) this->AttachMenuBar(menubar);
	m_menubar = menubar;
}

//-----------------------------------------------------------------------------
//  FbAuiMDIChildFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbAuiMDIChildFrame, wxAuiMDIChildFrame)
	EVT_ACTIVATE(FbAuiMDIChildFrame::OnActivated)
END_EVENT_TABLE()

FbAuiMDIChildFrame::FbAuiMDIChildFrame(wxAuiMDIParentFrame *parent, wxWindowID winid, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: wxAuiMDIChildFrame(parent, winid, title, pos, size, style, name)
{
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

wxMenuBar * FbAuiMDIChildFrame::CreateMenuBar()
{
    return new FbMainMenu;
}

void FbAuiMDIChildFrame::Localize(bool bUpdateMenu)
{
	if (bUpdateMenu) UpdateMenu();
}

void FbAuiMDIChildFrame::OnActivated(wxActivateEvent & event)
{
	if (event.GetActive()) {
		UpdateMenu();
	} else {
		FbAuiMDIParentFrame * parent = (FbAuiMDIParentFrame*) GetParent()->GetParent();
		parent->SetMainMenu(new FbMainMenu);
	}
	event.Skip();
}

void FbAuiMDIChildFrame::UpdateMenu()
{
	FbAuiMDIParentFrame * parent = (FbAuiMDIParentFrame*) GetParent()->GetParent();
	parent->SetMainMenu(this->CreateMenuBar());
}
