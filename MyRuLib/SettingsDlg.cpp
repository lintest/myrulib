///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "FbParams.h"
#include "SettingsDlg.h"
#include "XpmBitmaps.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SettingsDlg, wxDialog )
	EVT_BUTTON( ID_FB2_PROGRAM_BTN, SettingsDlg::OnSelectFileClick )
	EVT_BUTTON( ID_LIBRARY_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_DOWNLOAD_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTRACT_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTERNAL_BTN, SettingsDlg::OnSelectFolderClick )
END_EVENT_TABLE()

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
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizerTop11;
	fgSizerTop11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop11->SetFlexibleDirection( wxBOTH );
	fgSizerTop11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Описание библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizerTop11->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl1 = new wxTextCtrl( m_panel1, ID_LIBRARY_TITLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 200,-1 ) );

	fgSizerTop11->Add( m_textCtrl1, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, wxT("Программа для\nчтения книг FB2:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizerTop11->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl5 = new wxTextCtrl( m_panel1, ID_FB2_PROGRAM_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl5->SetMinSize( wxSize( 300,-1 ) );

	fgSizer15->Add( m_textCtrl5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton5 = new wxBitmapButton( m_panel1, ID_FB2_PROGRAM_BTN, wxBitmap( exefile_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer15->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( fgSizer15, 1, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Папка с файлами zip\nбиблиотеки lib.rus.ec:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizerTop11->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl2 = new wxTextCtrl( m_panel1, ID_LIBRARY_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->SetMinSize( wxSize( 300,-1 ) );

	fgSizer12->Add( m_textCtrl2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton2 = new wxBitmapButton( m_panel1, ID_LIBRARY_DIR_BTN, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer12->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( fgSizer12, 1, wxEXPAND, 5 );

	m_staticText31 = new wxStaticText( m_panel1, wxID_ANY, wxT("Файлы скаченные\nчерез интернет:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizerTop11->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl3 = new wxTextCtrl( m_panel1, ID_DOWNLOAD_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl3->SetMinSize( wxSize( 300,-1 ) );

	fgSizer13->Add( m_textCtrl3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton3 = new wxBitmapButton( m_panel1, ID_DOWNLOAD_DIR_BTN, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer13->Add( m_bpButton3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( fgSizer13, 1, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, wxT("Папка для хранения\nвременных файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizerTop11->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl4 = new wxTextCtrl( m_panel1, ID_EXTRACT_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl4->SetMinSize( wxSize( 300,-1 ) );

	fgSizer14->Add( m_textCtrl4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton4 = new wxBitmapButton( m_panel1, ID_EXTRACT_DIR_BTN, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer14->Add( m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizerTop11->Add( fgSizer14, 1, wxEXPAND, 5 );

	fgSizer1->Add( fgSizerTop11, 1, wxEXPAND|wxTOP, 5 );

	m_checkBox1 = new wxCheckBox( m_panel1, ID_EXTRACT_DELETE, wxT("Удалять все временные файлы при выходе из программы."), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer1->Add( m_checkBox1, 0, wxALL, 5 );

	m_panel1->SetSizer( fgSizer1 );
	m_panel1->Layout();
	fgSizer1->Fit( m_panel1 );
	m_notebook->AddPage( m_panel1, wxT("Папки и файлы"), true );
	m_panel2 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, wxT("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer21->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer22;
	fgSizer22 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer22->SetFlexibleDirection( wxBOTH );
	fgSizer22->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl6 = new wxTextCtrl( m_panel2, ID_EXTERNAL_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	fgSizer22->Add( m_textCtrl6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_bpButton6 = new wxBitmapButton( m_panel2, ID_EXTERNAL_BTN, wxBitmap( folder_open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer22->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	fgSizer21->Add( fgSizer22, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer2->Add( fgSizer21, 0, wxEXPAND|wxTOP, 5 );

	wxFlexGridSizer* fgSizer23;
	fgSizer23 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer23->SetFlexibleDirection( wxBOTH );
	fgSizer23->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox2 = new wxCheckBox( m_panel2, ID_TRANSLIT_FOLDER, wxT("Транслитерация имени папки"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer23->Add( m_checkBox2, 0, wxALL, 5 );

	m_checkBox3 = new wxCheckBox( m_panel2, ID_TRANSLIT_FILE, wxT("Транслитерация имен файлов"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer23->Add( m_checkBox3, 0, wxALL, 5 );

	wxString m_radioBox1Choices[] = { wxT("Автор / Книга"), wxT("Автор / Серия / Книга") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( m_panel2, ID_FOLDER_FORMAT, wxT("Организация папок и файлов"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	fgSizer23->Add( m_radioBox1, 0, wxALL, 5 );

	wxString m_radioBox2Choices[] = { wxT("filename.fb2"), wxT("filename.fb2.zip") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_panel2, ID_FILE_FORMAT, wxT("Формат выгрузки"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox2->SetSelection( 0 );
	fgSizer23->Add( m_radioBox2, 0, wxALL, 5 );

	fgSizer2->Add( fgSizer23, 0, wxEXPAND, 5 );

	m_panel2->SetSizer( fgSizer2 );
	m_panel2->Layout();
	fgSizer2->Fit( m_panel2 );
	m_notebook->AddPage( m_panel2, wxT("Внешнее устройство"), false );
	m_panel3 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox21 = new wxCheckBox( m_panel3, ID_USE_PROXY, wxT("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer3->Add( m_checkBox21, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText7 = new wxStaticText( m_panel3, wxID_ANY, wxT("    Адрес прокси-сервера:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer31->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizerProxy32;
	fgSizerProxy32 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizerProxy32->SetFlexibleDirection( wxBOTH );
	fgSizerProxy32->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl7 = new wxTextCtrl( m_panel3, ID_PROXY_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl7->SetMinSize( wxSize( 180,-1 ) );

	fgSizerProxy32->Add( m_textCtrl7, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText8 = new wxStaticText( m_panel3, wxID_ANY, wxT("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizerProxy32->Add( m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl8 = new wxTextCtrl( m_panel3, ID_PROXY_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl8->SetMinSize( wxSize( 60,-1 ) );

	fgSizerProxy32->Add( m_textCtrl8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer31->Add( fgSizerProxy32, 1, wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel3, wxID_ANY, wxT("    Имя пользователя:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer31->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFlexGridSizer* fgSizerProxy33;
	fgSizerProxy33 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizerProxy33->SetFlexibleDirection( wxBOTH );
	fgSizerProxy33->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_textCtrl9 = new wxTextCtrl( m_panel3, ID_PROXY_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl9->SetMinSize( wxSize( 110,-1 ) );

	fgSizerProxy33->Add( m_textCtrl9, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_staticText10 = new wxStaticText( m_panel3, wxID_ANY, wxT("Пароль:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizerProxy33->Add( m_staticText10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl10 = new wxTextCtrl( m_panel3, ID_PROXY_PASS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_textCtrl10->SetMinSize( wxSize( 110,-1 ) );

	fgSizerProxy33->Add( m_textCtrl10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer31->Add( fgSizerProxy33, 1, wxEXPAND, 5 );

	fgSizer3->Add( fgSizer31, 1, wxEXPAND, 5 );

	m_panel3->SetSizer( fgSizer3 );
	m_panel3->Layout();
	fgSizer3->Fit( m_panel3 );
	m_notebook->AddPage( m_panel3, wxT("Настройки интернет"), false );

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
    wxDirDialog dlg(
        this,
        _("Выберите директорию"),
        wxEmptyString,
        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST
    );

	if (dlg.ShowModal() == wxID_OK) {
	    wxTextCtrl * textCtrl = (wxTextCtrl*)FindWindowById( event.GetId() - 1);
	    if (textCtrl) textCtrl->SetValue(dlg.GetPath());
	}

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
        {ID_USE_PROXY, SettingsDlg::ID_USE_PROXY, tCheck},
        {FB_PROXY_ADDR, SettingsDlg::ID_PROXY_ADDR, tText},
        {FB_PROXY_PORT, SettingsDlg::ID_PROXY_PORT, tText},
        {FB_PROXY_NAME, SettingsDlg::ID_PROXY_NAME, tText},
        {FB_PROXY_PASS, SettingsDlg::ID_PROXY_PASS, tText},
    };

    const size_t idsCount = sizeof(ids) / sizeof(Struct);

    FbParams params;

    for (size_t i=0; i<idsCount; i++) {
        switch (ids[i].type) {
            case tText: {
                wxTextCtrl * control = (wxTextCtrl*)FindWindowById(ids[i].control);
                if (control) {
                    if (write)
                        params.SetText(ids[i].param, control->GetValue());
                    else
                        control->SetValue(params.GetText(ids[i].param));
                }
            } break;
            case tCheck: {
                wxCheckBox * control = (wxCheckBox*)FindWindowById(ids[i].control);
                if (control) {
                    if (write)
                        params.SetValue(ids[i].param, control->GetValue());
                    else
                        control->SetValue(params.GetValue(ids[i].param));
                }
            } break;
            case tRadio: {
                wxRadioBox * control = (wxRadioBox*)FindWindowById(ids[i].control);
                if (control) {
                    if (write)
                        params.SetValue(ids[i].param, control->GetSelection());
                    else
                        control->SetSelection(params.GetValue(ids[i].param));
                }
            } break;
        }

    }
};

void SettingsDlg::Execute(wxWindow* parent)
{
    SettingsDlg dlg(parent, wxID_ANY, _("Настройка параметров программы"), wxDefaultPosition, wxDefaultSize);
    dlg.Assign(false);
    if (dlg.ShowModal() == wxID_OK) dlg.Assign(true);
};
