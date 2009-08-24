///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/artprov.h>
#include "FbParams.h"
#include "SettingsDlg.h"
#include "ZipReader.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SettingsDlg, wxDialog )
	EVT_BUTTON( ID_FB2_PROGRAM_BTN, SettingsDlg::OnSelectFileClick )
	EVT_BUTTON( ID_LIBRARY_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_DOWNLOAD_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTRACT_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTERNAL_BTN, SettingsDlg::OnSelectFolderClick )
END_EVENT_TABLE()

/*
wxBitmap( wxT("exefile_xpm"), wxBITMAP_TYPE_RESOURCE ) wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE)
wxBitmap( wxT("folder_open_xpm"), wxBITMAP_TYPE_RESOURCE ) wxArtProvider::GetBitmap(wxART_FOLDER_OPEN)
*/

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    wxNotebook* m_notebook;
    wxPanel* m_panel1;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCtrl1;
    wxStaticText* m_staticText5;
    wxTextCtrl* m_textCtrl5;
    wxBitmapButton* m_bpButton5;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_textCtrl2;
    wxBitmapButton* m_bpButton2;
    wxStaticText* m_staticText31;
    wxTextCtrl* m_textCtrl3;
    wxBitmapButton* m_bpButton3;
    wxStaticText* m_staticText4;
    wxTextCtrl* m_textCtrl4;
    wxBitmapButton* m_bpButton4;
    wxCheckBox* m_checkBox1;
    wxPanel* m_panel2;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textCtrl6;
    wxBitmapButton* m_bpButton6;
    wxCheckBox* m_checkBox2;
    wxCheckBox* m_checkBox3;
    wxRadioBox* m_radioBox1;
    wxRadioBox* m_radioBox2;
    wxPanel* m_panel3;
    wxCheckBox* m_checkBox21;
    wxStaticText* m_staticText7;
    wxTextCtrl* m_textCtrl7;
    wxStaticText* m_staticText8;
    wxTextCtrl* m_textCtrl8;
    wxStaticText* m_staticText9;
    wxTextCtrl* m_textCtrl9;
    wxStaticText* m_staticText10;
    wxTextCtrl* m_textCtrl10;
    wxStdDialogButtonSizer* m_sdbSizerBtn;
    wxButton* m_sdbSizerBtnOK;
    wxButton* m_sdbSizerBtnCancel;

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizerTop11;
	fgSizerTop11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop11->SetFlexibleDirection( wxBOTH );
	fgSizerTop11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, _("Описание библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizerTop11->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl1 = new wxTextCtrl( m_panel1, ID_LIBRARY_TITLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 200,-1 ) );

	fgSizerTop11->Add( m_textCtrl1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, _("Программа для\nчтения книг FB2:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizerTop11->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl5 = new wxTextCtrl( m_panel1, ID_FB2_PROGRAM_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl5->SetMinSize( wxSize( 300,-1 ) );

	bSizer3->Add( m_textCtrl5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton5 = new wxBitmapButton( m_panel1, ID_FB2_PROGRAM_BTN, wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer3->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( bSizer3, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Папка с файлами zip\nбиблиотеки lib.rus.ec:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizerTop11->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl2 = new wxTextCtrl( m_panel1, ID_LIBRARY_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->SetMinSize( wxSize( 300,-1 ) );

	bSizer4->Add( m_textCtrl2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton2 = new wxBitmapButton( m_panel1, ID_LIBRARY_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer4->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( bSizer4, 1, wxEXPAND, 5 );

	m_staticText31 = new wxStaticText( m_panel1, wxID_ANY, _("Файлы скаченные\nчерез интернет:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizerTop11->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl3 = new wxTextCtrl( m_panel1, ID_DOWNLOAD_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl3->SetMinSize( wxSize( 300,-1 ) );

	bSizer5->Add( m_textCtrl3, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton3 = new wxBitmapButton( m_panel1, ID_DOWNLOAD_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer5->Add( m_bpButton3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( bSizer5, 1, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, _("Папка для хранения\nвременных файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizerTop11->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl4 = new wxTextCtrl( m_panel1, ID_EXTRACT_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl4->SetMinSize( wxSize( 300,-1 ) );

	bSizer6->Add( m_textCtrl4, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton4 = new wxBitmapButton( m_panel1, ID_EXTRACT_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer6->Add( m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( bSizer6, 1, wxEXPAND, 5 );

	bSizer1->Add( fgSizerTop11, 1, wxEXPAND|wxTOP, 5 );

	m_checkBox1 = new wxCheckBox( m_panel1, ID_EXTRACT_DELETE, _("Удалять все временные файлы при выходе из программы."), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer1->Add( m_checkBox1, 0, wxALL|wxEXPAND, 5 );

	m_panel1->SetSizer( bSizer1 );
	m_panel1->Layout();
	bSizer1->Fit( m_panel1 );
	m_notebook->AddPage( m_panel1, _("Папки и файлы"), true );
	m_panel2 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, _("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer8->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_textCtrl6 = new wxTextCtrl( m_panel2, ID_EXTERNAL_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	bSizer8->Add( m_textCtrl6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton6 = new wxBitmapButton( m_panel2, ID_EXTERNAL_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer8->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer12->Add( bSizer8, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer23;
	fgSizer23 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer23->SetFlexibleDirection( wxBOTH );
	fgSizer23->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox2 = new wxCheckBox( m_panel2, ID_TRANSLIT_FOLDER, _("Транслитерация имени папки"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer23->Add( m_checkBox2, 0, wxALL, 5 );

	m_checkBox3 = new wxCheckBox( m_panel2, ID_TRANSLIT_FILE, _("Транслитерация имен файлов"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer23->Add( m_checkBox3, 0, wxALL, 5 );

	wxString m_radioBox1Choices[] = { _("Автор / Книга"), _("Автор / Серия / Книга") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( m_panel2, ID_FOLDER_FORMAT, _("Организация папок и файлов"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	fgSizer23->Add( m_radioBox1, 0, wxALL, 5 );

	wxString m_radioBox2Choices[] = { _("filename.fb2"), _("filename.fb2.zip") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_panel2, ID_FILE_FORMAT, _("Формат выгрузки"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox2->SetSelection( 0 );
	fgSizer23->Add( m_radioBox2, 0, wxALL, 5 );

	bSizer12->Add( fgSizer23, 0, wxEXPAND, 5 );

	m_panel2->SetSizer( bSizer12 );
	m_panel2->Layout();
	bSizer12->Fit( m_panel2 );
	m_notebook->AddPage( m_panel2, _("Внешнее устройство"), false );
	m_panel3 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox21 = new wxCheckBox( m_panel3, ID_USE_PROXY, _("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer3->Add( m_checkBox21, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText7 = new wxStaticText( m_panel3, wxID_ANY, _("    Адрес прокси-сервера:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer16->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl7 = new wxTextCtrl( m_panel3, ID_PROXY_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl7->SetMinSize( wxSize( 100,-1 ) );

	bSizer13->Add( m_textCtrl7, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText8 = new wxStaticText( m_panel3, wxID_ANY, _("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer13->Add( m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl8 = new wxTextCtrl( m_panel3, ID_PROXY_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl8->SetMinSize( wxSize( 60,-1 ) );

	bSizer13->Add( m_textCtrl8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer16->Add( bSizer13, 0, wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel3, wxID_ANY, _("    Имя пользователя:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer16->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl9 = new wxTextCtrl( m_panel3, ID_PROXY_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl9->SetMinSize( wxSize( 110,-1 ) );

	bSizer14->Add( m_textCtrl9, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText10 = new wxStaticText( m_panel3, wxID_ANY, _("Пароль:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer14->Add( m_staticText10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl10 = new wxTextCtrl( m_panel3, ID_PROXY_PASS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_textCtrl10->SetMinSize( wxSize( 110,-1 ) );

	bSizer14->Add( m_textCtrl10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer16->Add( bSizer14, 0, wxEXPAND, 5 );

	fgSizer3->Add( fgSizer16, 0, wxEXPAND, 5 );

	m_panel3->SetSizer( fgSizer3 );
	m_panel3->Layout();
	fgSizer3->Fit( m_panel3 );
	m_notebook->AddPage( m_panel3, _("Настройки интернет"), false );

	fgSizerMain->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizerBtn = new wxStdDialogButtonSizer();
	m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	fgSizerMain->Add( m_sdbSizerBtn, 1, wxEXPAND|wxALL, 5 );

	this->SetSizer( fgSizerMain );
	this->Layout();
	fgSizerMain->Fit( this );
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnSelectFileClick( wxCommandEvent& event )
{
#ifdef __WIN32__
    wxString wildCard = _("Исполняемые файлы (*.exe)|*.exe");
#else
    wxString wildCard = wxFileSelectorDefaultWildcardStr;
#endif

    wxFileDialog dlg (
		this,
		_("Выберите приложение для просмотра FB2 файлов…"),
		wxEmptyString,
		wxEmptyString,
		wildCard,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
	    wxTextCtrl * textCtrl = (wxTextCtrl*)FindWindowById( event.GetId() - 1);
	    if (textCtrl) textCtrl->SetValue(dlg.GetPath());
	}
}

void SettingsDlg::OnSelectFolderClick( wxCommandEvent& event )
{
    wxTextCtrl * textCtrl = (wxTextCtrl*)FindWindowById( event.GetId() - 1);

    if (!textCtrl) return;

    wxDirDialog dlg(
        this,
        _("Выберите директорию"),
        textCtrl->GetValue(),
        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
    );

	if (dlg.ShowModal() == wxID_OK)  textCtrl->SetValue(dlg.GetPath());
}

void SettingsDlg::Assign(bool write)
{
    enum Type {
        tText,
        tCheck,
        tRadio,
    };
    struct Struct{
        int param;
        ID control;
        Type type;
    };

    const Struct ids[] = {
        {DB_LIBRARY_TITLE, SettingsDlg::ID_LIBRARY_TITLE, tText},
        {FB_FB2_PROGRAM, SettingsDlg::ID_FB2_PROGRAM_TXT, tText},
        {FB_LIBRARY_DIR, SettingsDlg::ID_LIBRARY_DIR_TXT, tText},
        {FB_DOWNLOAD_DIR, SettingsDlg::ID_DOWNLOAD_DIR_TXT, tText},
        {FB_EXTRACT_DIR, SettingsDlg::ID_EXTRACT_DIR_TXT, tText},
        {FB_EXTRACT_DELETE, SettingsDlg::ID_EXTRACT_DELETE, tCheck},
        {FB_EXTERNAL_DIR, SettingsDlg::ID_EXTERNAL_TXT, tText},
        {FB_TRANSLIT_FOLDER, SettingsDlg::ID_TRANSLIT_FOLDER, tCheck},
        {FB_TRANSLIT_FILE, SettingsDlg::ID_TRANSLIT_FILE, tCheck},
        {FB_FOLDER_FORMAT, SettingsDlg::ID_FOLDER_FORMAT, tRadio},
        {FB_FILE_FORMAT, SettingsDlg::ID_FILE_FORMAT, tRadio},
        {FB_USE_PROXY, SettingsDlg::ID_USE_PROXY, tCheck},
        {FB_PROXY_ADDR, SettingsDlg::ID_PROXY_ADDR, tText},
        {FB_PROXY_PORT, SettingsDlg::ID_PROXY_PORT, tText},
        {FB_PROXY_NAME, SettingsDlg::ID_PROXY_NAME, tText},
        {FB_PROXY_PASS, SettingsDlg::ID_PROXY_PASS, tText},
    };

    const size_t idsCount = sizeof(ids) / sizeof(Struct);

    FbParams params;

    for (size_t i=0; i<idsCount; i++) {
        switch (ids[i].type) {
            case tText:
                if (wxTextCtrl * control = (wxTextCtrl*)FindWindowById(ids[i].control))
                    if (write)
                        params.SetText(ids[i].param, control->GetValue());
                    else
                        control->SetValue(params.GetText(ids[i].param));
				break;
            case tCheck:
                if (wxCheckBox * control = (wxCheckBox*)FindWindowById(ids[i].control))
                    if (write)
                        params.SetValue(ids[i].param, control->GetValue());
                    else
                        control->SetValue((bool)params.GetValue(ids[i].param));
				break;
            case tRadio:
                if (wxRadioBox * control = (wxRadioBox*)FindWindowById(ids[i].control))
                    if (write)
                        params.SetValue(ids[i].param, control->GetSelection());
                    else
                        control->SetSelection(params.GetValue(ids[i].param));
				break;
        }

    }

	if (write) ZipReader::Init();
};

void SettingsDlg::Execute(wxWindow* parent)
{
    SettingsDlg dlg(parent, wxID_ANY, _("Настройка параметров программы"), wxDefaultPosition, wxDefaultSize);
    dlg.Assign(false);
    if (dlg.ShowModal() == wxID_OK) dlg.Assign(true);
};
