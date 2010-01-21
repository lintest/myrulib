#include "FbAboutDlg.h"
#include <wx/artprov.h>
#include "FbLogoBitmap.h"
#include "FbConst.h"
#include "MyRuLibApp.h"

FbAboutDlg::FbAboutDlg( wxWindow* parent )
	: FbDialog(parent, wxID_ANY, (wxString)_("О программе…"))
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerTop;
	bSizerTop = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBitmap * m_bitmap = new wxStaticBitmap( this, wxID_ANY, FbLogoBitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTop->Add( m_bitmap, 0, wxALL, 10 );

	wxBoxSizer* bSizerCtrl;
	bSizerCtrl = new wxBoxSizer( wxVERTICAL );

	wxStaticText * m_text1 = new wxStaticText( this, wxID_ANY, strProgramName + wxT(" (alpha)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_text1->Wrap( -1 );
	wxFont font;
	font.SetWeight(wxFONTWEIGHT_BOLD);
	m_text1->SetFont( font );

	bSizerCtrl->Add( m_text1, 0, wxALL|wxEXPAND, 10 );

	wxStaticText * m_text2 = new wxStaticText( this, wxID_ANY, strVersionInfo, wxDefaultPosition, wxDefaultSize, 0 );
	m_text2->Wrap( -1 );
	bSizerCtrl->Add( m_text2, 0, wxALL|wxEXPAND, 10 );

	wxStaticText * m_text3 = new wxStaticText( this, wxID_ANY, wxGetApp().GetAppData(), wxDefaultPosition, wxDefaultSize, 0 );
	m_text3->Wrap( -1 );
	bSizerCtrl->Add( m_text3, 0, wxALL|wxEXPAND, 10 );

	wxStaticText * m_text4 = new wxStaticText( this, wxID_ANY, FbDatabase::GetConfigName(), wxDefaultPosition, wxDefaultSize, 0 );
	m_text3->Wrap( -1 );
	bSizerCtrl->Add( m_text4, 0, wxALL|wxEXPAND, 10 );

	bSizerTop->Add( bSizerCtrl, 1, wxEXPAND, 5 );

	bSizerMain->Add( bSizerTop, 0, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

