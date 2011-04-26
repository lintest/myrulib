#include "FbEditBook.h"
#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

FbEditBookDlg::FbEditBookDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizerGrid;
	fgSizerGrid = new wxFlexGridSizer( 2, 0, 0 );
	fgSizerGrid->AddGrowableCol( 1 );
	fgSizerGrid->SetFlexibleDirection( wxBOTH );
	fgSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	AppenTitle(fgSizerGrid);

	AppenItem(fgSizerGrid, ID_AUTHOR_CHECK,   _("Author"));
	AppenItem(fgSizerGrid, ID_SEQUENCE_CHECK, _("Series"));
	AppenItem(fgSizerGrid, ID_GENRE_CHECK,    _("Genre"));

	bSizerMain->Add( fgSizerGrid, 1, wxEXPAND, 5 );

//	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
//	bSizerMain->Add( panel, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * m_sdbSizerBtn = new wxStdDialogButtonSizer();
	wxButton * m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnOK->SetDefault();
	wxButton * m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	bSizerMain->Add( m_sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	wxSize newSize = GetBestSize();
	this->SetSize(newSize);
}

FbEditBookDlg::~FbEditBookDlg()
{
}

void FbEditBookDlg::AppenTitle(wxFlexGridSizer * sizer)
{
	wxCheckBox * cbTitle = new wxCheckBox( this, ID_TITLE_CHECK, _("Title"), wxDefaultPosition, wxDefaultSize, 0 );
	sizer->Add( cbTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxTextCtrl * tcTitle = new wxTextCtrl( this, ID_TITLE_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	tcTitle->SetMinSize( wxSize( 200,-1 ) );
	sizer->Add( tcTitle, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
}

void FbEditBookDlg::AppenItem(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption)
{
	wxCheckBox * check = new wxCheckBox( this, id, caption, wxDefaultPosition, wxDefaultSize, 0 );
	parent->Add( check, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer * sizer = new wxBoxSizer( wxHORIZONTAL );

	wxTextCtrl * text = new wxTextCtrl( this, ++id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	text->SetMinSize( wxSize( 300,-1 ) );
	sizer->Add( text, 1, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	wxBitmapButton * button = new wxBitmapButton( this, ++id, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	sizer->Add( button, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	parent->Add( sizer, 0, wxEXPAND, 5 );
}

bool FbEditBookDlg::Execute()
{
	FbEditBookDlg dlg(NULL);
	bool result = dlg.ShowModal() == wxID_OK;
	return result;
}
