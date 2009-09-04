#include "FbFrameSearch.h"

FbFrameSearch::FbFrameSearch()
{
}

FbFrameSearch::FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	Create(parent, id, title);
}

bool FbFrameSearch::Create(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if(res) CreateControls();
	return res;
}

void FbFrameSearch::CreateControls()
{
	SetSizer(new wxBoxSizer(wxVERTICAL));
	SetMenuBar(CreateMenuBar());
}

wxMenuBar * FbFrameSearch::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;

	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(wxID_EXIT, _("Exit\tAlt+F4"));
	menuBar->Append(fileMenu, _("File"));

	wxMenu * helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, _("About..."));
	menuBar->Append(helpMenu, _("Help"));

	return menuBar;
}

void FbFrameSearch::OnActivated(wxActivateEvent & event)
{
    /*
	AUIDocViewMainFrame * frame = wxDynamicCast(GetMDIParentFrame(),
		AUIDocViewMainFrame);
	if(frame)
	{
		frame->GetLOGTextCtrl()->SetValue(wxString::Format(
			_("Some help text about '%s'"),	GetTitle().GetData()));
	}
	*/
}
