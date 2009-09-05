#include "FbFrameSearch.h"
#include <wx/artprov.h>

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
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxStaticText* m_staticText1;
    wxStaticText* m_staticText2;
    wxToolBar* m_toolBar1;

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Наименование:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textTitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textTitle, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Автор:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textAuthor = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textAuthor, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_toolBar1 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT );
	m_toolBar1->AddTool( wxID_ANY, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();

	bSizer2->Add( m_toolBar1, 0, wxEXPAND|wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

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
