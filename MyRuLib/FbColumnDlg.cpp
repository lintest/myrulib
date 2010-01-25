#include "FbColumnDlg.h"

///////////////////////////////////////////////////////////////////////////

FbColumnDlg::FbColumnDlg( wxWindow* parent, wxWindowID id)
	: FbDialog( parent, id, _("Видимость колонок"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxCheckBox * m_checkBox1 = new wxCheckBox( this, wxID_ANY, _("Check Me!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_checkBox1, 0, wxALL, 5 );

	wxCheckBox * m_checkBox2 = new wxCheckBox( this, wxID_ANY, _("Check Me!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_checkBox2, 0, wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
}

