#include <wx/artprov.h>
#include "FbTitleDlg.h"
#include "res/add.xpm"
#include "res/del.xpm"

FbTitleDlg::AuthSubPanel::AuthSubPanel( wxWindow* parent)
	: wxPanel( parent )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );
	
	m_text.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	
	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER ); 
	m_toolbar.AddTool( wxID_ADD, _("Append"), wxBitmap(add_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_toolbar.AddTool( wxID_DELETE, _("Delete"), wxBitmap(del_xpm), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_toolbar.Realize(); 
	
	bSizerMain->Add( &m_toolbar, 0, wxALIGN_CENTER_VERTICAL, 3 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

FbTitleDlg::TitlePanel::TitlePanel( wxWindow* parent)
	: wxPanel( parent )
{
	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;
	
	info = new wxStaticText( this, wxID_ANY, _("Book title"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );
	
	m_title.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerMain->Add( &m_title, 0, wxALL|wxEXPAND, 3 );
	m_title.SetMinSize( wxSize( 300, -1 ) );
	
	info = new wxStaticText( this, wxID_ANY, _("Authors"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );
	
	m_authors = new wxBoxSizer(wxVERTICAL);
	m_authors->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	m_authors->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	m_authors->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_authors, 1, wxEXPAND | wxRIGHT, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Series"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_series = new wxBoxSizer(wxVERTICAL);
	m_series->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	m_series->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_series, 1, wxEXPAND | wxRIGHT, 5 );
	
	info = new wxStaticText( this, wxID_ANY, _("Genres"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	fgSizerMain->Add( info, 0, wxALL, 5 );

	m_genres = new wxBoxSizer(wxVERTICAL);
	m_genres->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	m_genres->Add( new AuthSubPanel(this), 1, wxEXPAND, 5 );
	fgSizerMain->Add( m_genres, 1, wxEXPAND | wxRIGHT, 5 );
	
	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

bool FbTitleDlg::Execute(int book)
{
	FbTitleDlg dlg(NULL, wxID_ANY, _("Properties"));
	dlg.SetSize(dlg.GetBestSize());
	return dlg.ShowModal() == wxID_OK;
}

FbTitleDlg::FbTitleDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
	: FbDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	sizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxPanel * panel = new TitlePanel( m_notebook );
	m_notebook->AddPage( panel, _("Title"), false );
	
	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( sizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

FbTitleDlg::~FbTitleDlg()
{
}

