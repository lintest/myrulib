///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SettingsDlg.h"
#include "res/folder_open.xpm"
#include "res/exefile.xpm"

///////////////////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Описание библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer7->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl5 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl5->SetMinSize( wxSize( 200,-1 ) );

	fgSizer7->Add( m_textCtrl5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Папка с файлами zip\nбиблиотеки lib.rus.ec:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer7->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl6 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	fgSizer9->Add( m_textCtrl6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton2 = new wxBitmapButton( this, wxID_ANY, wxBitmap(folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer9->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer7->Add( fgSizer9, 1, wxEXPAND, 5 );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Папка для хранения\nвременных файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer7->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl7 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl7->SetMinSize( wxSize( 300,-1 ) );

	fgSizer10->Add( m_textCtrl7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton3 = new wxBitmapButton( this, wxID_ANY, wxBitmap(folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer10->Add( m_bpButton3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer7->Add( fgSizer10, 1, wxEXPAND, 5 );

	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Файлы скаченные\nчерез интернет:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer7->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl11 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl11->SetMinSize( wxSize( 300,-1 ) );

	fgSizer17->Add( m_textCtrl11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton5 = new wxBitmapButton( this, wxID_ANY, wxBitmap(folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer17->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer7->Add( fgSizer17, 1, wxEXPAND, 5 );

	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer7->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl12 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl12->SetMinSize( wxSize( 300,-1 ) );

	fgSizer16->Add( m_textCtrl12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton6 = new wxBitmapButton( this, wxID_ANY, wxBitmap(folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer16->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer7->Add( fgSizer16, 1, wxEXPAND, 5 );

	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("Программа для\nчтения книг FB2:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer7->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl8 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl8->SetMinSize( wxSize( 300,-1 ) );

	fgSizer11->Add( m_textCtrl8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton4 = new wxBitmapButton( this, wxID_ANY, wxBitmap(exefile_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer11->Add( m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer7->Add( fgSizer11, 1, wxEXPAND, 5 );

	fgSizer6->Add( fgSizer7, 1, wxEXPAND|wxALL, 5 );

	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("Удалять все временные файлы при выходе из программы."), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer6->Add( m_checkBox1, 0, wxALL, 5 );

	m_checkBox2 = new wxCheckBox( this, wxID_ANY, wxT("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer6->Add( m_checkBox2, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("    Адрес прокси-сервера:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer15->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl9 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl9->SetMinSize( wxSize( 180,-1 ) );

	fgSizer15->Add( m_textCtrl9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer15->Add( m_staticText11, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl10 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer15->Add( m_textCtrl10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer6->Add( fgSizer15, 1, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer6->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	fgSizer6->Add( m_sdbSizer2, 1, wxEXPAND|wxALL, 5 );

	this->SetSizer( fgSizer6 );
	this->Layout();
	fgSizer6->Fit( this );
}

SettingsDlg::~SettingsDlg()
{
}
