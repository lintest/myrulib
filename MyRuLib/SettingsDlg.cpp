///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SettingsDlg.h"
#include "XpmBitmaps.h"


///////////////////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer25;
	fgSizer25 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer25->SetFlexibleDirection( wxBOTH );
	fgSizer25->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizerTop;
	fgSizerTop = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop->SetFlexibleDirection( wxBOTH );
	fgSizerTop->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Описание библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizerTop->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl1 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 200,-1 ) );

	fgSizerTop->Add( m_textCtrl1, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Папка с файлами zip\nбиблиотеки lib.rus.ec:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizerTop->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer101;
	fgSizer101 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer101->SetFlexibleDirection( wxBOTH );
	fgSizer101->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl3 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl3->SetMinSize( wxSize( 300,-1 ) );

	fgSizer101->Add( m_textCtrl3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton2 = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer101->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop->Add( fgSizer101, 1, wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( m_panel1, wxID_ANY, wxT("Файлы скаченные\nчерез интернет:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizerTop->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer91->SetFlexibleDirection( wxBOTH );
	fgSizer91->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl2 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->SetMinSize( wxSize( 300,-1 ) );

	fgSizer91->Add( m_textCtrl2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton1 = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer91->Add( m_bpButton1, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop->Add( fgSizer91, 1, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, wxT("Папка для хранения\nвременных файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizerTop->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl4 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl4->SetMinSize( wxSize( 300,-1 ) );

	fgSizer17->Add( m_textCtrl4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton3 = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer17->Add( m_bpButton3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop->Add( fgSizer17, 1, wxEXPAND, 5 );

	m_staticText16 = new wxStaticText( m_panel1, wxID_ANY, wxT("Программа для\nчтения книг FB2:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizerTop->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl6 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	fgSizer11->Add( m_textCtrl6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton5 = new wxBitmapButton( m_panel1, wxID_ANY, wxBitmap( exefile_xpm ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer11->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop->Add( fgSizer11, 1, wxEXPAND, 5 );

	fgSizer25->Add( fgSizerTop, 1, wxEXPAND|wxTOP, 5 );

	m_checkBox1 = new wxCheckBox( m_panel1, wxID_ANY, wxT("Удалять все временные файлы при выходе из программы."), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer25->Add( m_checkBox1, 0, wxALL, 5 );

	m_panel1->SetSizer( fgSizer25 );
	m_panel1->Layout();
	fgSizer25->Fit( m_panel1 );
	m_notebook->AddPage( m_panel1, wxT("Папки и файлы"), true );
	m_panel2 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer251;
	fgSizer251 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer251->SetFlexibleDirection( wxBOTH );
	fgSizer251->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizerTop1;
	fgSizerTop1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop1->SetFlexibleDirection( wxBOTH );
	fgSizerTop1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText51 = new wxStaticText( m_panel2, wxID_ANY, wxT("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizerTop1->Add( m_staticText51, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer161;
	fgSizer161 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer161->SetFlexibleDirection( wxBOTH );
	fgSizer161->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl51 = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl51->SetMinSize( wxSize( 300,-1 ) );

	fgSizer161->Add( m_textCtrl51, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_bpButton41 = new wxBitmapButton( m_panel2, wxID_ANY, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer161->Add( m_bpButton41, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop1->Add( fgSizer161, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer251->Add( fgSizerTop1, 0, wxEXPAND|wxTOP, 5 );

	wxFlexGridSizer* fgSizer95;
	fgSizer95 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer95->SetFlexibleDirection( wxBOTH );
	fgSizer95->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox11 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Транслитерация имени папки"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer95->Add( m_checkBox11, 0, wxALL, 5 );

	m_checkBox111 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Транслитерация имен файлов"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer95->Add( m_checkBox111, 0, wxALL, 5 );

	wxString m_radioBox2Choices[] = { wxT("Автор / Книга"), wxT("Автор / Серия / Книга") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_panel2, wxID_ANY, wxT("Организация папок и файлов"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox2->SetSelection( 0 );
	fgSizer95->Add( m_radioBox2, 0, wxALL, 5 );

	wxString m_radioBox21Choices[] = { wxT("filename.fb2"), wxT("filename.fb2.zip") };
	int m_radioBox21NChoices = sizeof( m_radioBox21Choices ) / sizeof( wxString );
	m_radioBox21 = new wxRadioBox( m_panel2, wxID_ANY, wxT("Формат выгрузки"), wxDefaultPosition, wxDefaultSize, m_radioBox21NChoices, m_radioBox21Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox21->SetSelection( 0 );
	fgSizer95->Add( m_radioBox21, 0, wxALL, 5 );

	fgSizer251->Add( fgSizer95, 0, wxEXPAND, 5 );

	m_checkBox1111 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Упаковывать файлы архиватором ZIP"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer251->Add( m_checkBox1111, 0, wxALL, 5 );

	m_panel2->SetSizer( fgSizer251 );
	m_panel2->Layout();
	fgSizer251->Fit( m_panel2 );
	m_notebook->AddPage( m_panel2, wxT("Внешнее устройство"), false );
	m_panel3 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer72;
	fgSizer72 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer72->SetFlexibleDirection( wxBOTH );
	fgSizer72->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer1611;
	fgSizer1611 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1611->SetFlexibleDirection( wxBOTH );
	fgSizer1611->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	fgSizer72->Add( fgSizer1611, 1, wxEXPAND, 5 );

	m_checkBox2 = new wxCheckBox( m_panel3, wxID_ANY, wxT("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer72->Add( m_checkBox2, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizerProxy;
	fgSizerProxy = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizerProxy->SetFlexibleDirection( wxBOTH );
	fgSizerProxy->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText7 = new wxStaticText( m_panel3, wxID_ANY, wxT("    Адрес прокси-сервера:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizerProxy->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl7 = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl7->SetMinSize( wxSize( 180,-1 ) );

	fgSizerProxy->Add( m_textCtrl7, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText8 = new wxStaticText( m_panel3, wxID_ANY, wxT("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizerProxy->Add( m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl8 = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerProxy->Add( m_textCtrl8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer72->Add( fgSizerProxy, 1, wxEXPAND, 5 );

	m_panel3->SetSizer( fgSizer72 );
	m_panel3->Layout();
	fgSizer72->Fit( m_panel3 );
	m_notebook->AddPage( m_panel3, wxT("Настройки интернет"), false );

	fgSizer10->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizer3 = new wxStdDialogButtonSizer();
	m_sdbSizer3OK = new wxButton( this, wxID_OK );
	m_sdbSizer3->AddButton( m_sdbSizer3OK );
	m_sdbSizer3Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer3->AddButton( m_sdbSizer3Cancel );
	m_sdbSizer3->Realize();
	fgSizer10->Add( m_sdbSizer3, 1, wxEXPAND|wxALL, 5 );

	fgSizer9->Add( fgSizer10, 1, wxEXPAND, 5 );

	this->SetSizer( fgSizer9 );
	this->Layout();
	fgSizer9->Fit( this );
}

SettingsDlg::~SettingsDlg()
{
}
