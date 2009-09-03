///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/artprov.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/textdlg.h>
#include "FbManager.h"
#include "FbParams.h"
#include "SettingsDlg.h"
#include "ZipReader.h"
#include "MyRuLibApp.h"
#include "db/Types.h"

///////////////////////////////////////////////////////////////////////////

class TypeListCtrl: public wxListCtrl
{
    public:
        TypeListCtrl( wxWindow *parent, wxWindowID winid = wxID_ANY, long style = wxLC_REPORT);
        void OnSize(wxSizeEvent& event);
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TypeListCtrl, wxListCtrl)
    EVT_SIZE(TypeListCtrl::OnSize)
END_EVENT_TABLE()

TypeListCtrl::TypeListCtrl( wxWindow *parent, wxWindowID winid, long style)
    :wxListCtrl( parent, winid, wxDefaultPosition, wxDefaultSize, style)
{
}

void TypeListCtrl::OnSize(wxSizeEvent& event)
{
	event.Skip();

	if (GetColumnCount() == 2){
        int w = GetClientSize().x - 20;
        SetColumnWidth(1, 50);
        SetColumnWidth(1, w - 50);
    }
}

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SettingsDlg, wxDialog )
	EVT_BUTTON( ID_LIBRARY_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( FB_WANRAIK_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTERNAL_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_MENU( ID_APPEND_TYPE, SettingsDlg::OnAppendType )
	EVT_MENU( ID_MODIFY_TYPE, SettingsDlg::OnModifyType )
	EVT_MENU( ID_DELETE_TYPE, SettingsDlg::OnDeleteType )
	EVT_LIST_ITEM_ACTIVATED(ID_TYPELIST, SettingsDlg::OnTypelistActivated)
END_EVENT_TABLE()

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    wxNotebook* m_notebook;
    wxPanel* m_panel1;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCtrl1;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_textCtrl2;
    wxBitmapButton* m_bpButton2;
    wxStaticText* m_staticText31;
    wxTextCtrl* m_textCtrl3;
    wxBitmapButton* m_bpButton3;
    wxPanel* m_panel2;
    wxStaticText* m_staticText6;
    wxTextCtrl* m_textCtrl6;
    wxBitmapButton* m_bpButton6;
    wxCheckBox* m_checkBox2;
    wxCheckBox* m_checkBox3;
    wxRadioBox* m_radioBox1;
    wxRadioBox* m_radioBox2;
    wxCheckBox* m_checkBox21;
    wxStaticText* m_staticText7;
    wxTextCtrl* m_textCtrl7;
    wxStaticText* m_staticText8;
    wxTextCtrl* m_textCtrl8;
    wxStaticText* m_staticText9;
    wxTextCtrl* m_textCtrl9;
    wxStaticText* m_staticText10;
    wxTextCtrl* m_textCtrl10;
	wxPanel* m_panel4;
	wxToolBar* m_tools;
    wxStdDialogButtonSizer* m_sdbSizerBtn;
    wxButton* m_sdbSizerBtnOK;
    wxButton* m_sdbSizerBtnCancel;

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, _("Описание библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Папка с файлами zip\nбиблиотеки lib.rus.ec:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText31 = new wxStaticText( m_panel1, wxID_ANY, _("Альтернативный путь\nк архивам библиотеки:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	bSizer4->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_textCtrl1 = new wxTextCtrl( m_panel1, ID_LIBRARY_TITLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 200,-1 ) );

	bSizer5->Add( m_textCtrl1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl3 = new wxTextCtrl( m_panel1, ID_LIBRARY_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl3->SetMinSize( wxSize( 300,-1 ) );

	bSizer6->Add( m_textCtrl3, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton3 = new wxBitmapButton( m_panel1, ID_LIBRARY_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer6->Add( m_bpButton3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl2 = new wxTextCtrl( m_panel1, FB_WANRAIK_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl2->SetMinSize( wxSize( 300,-1 ) );

	bSizer7->Add( m_textCtrl2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton2 = new wxBitmapButton( m_panel1, FB_WANRAIK_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer7->Add( m_bpButton2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer5->Add( bSizer7, 1, wxEXPAND, 5 );

	bSizer3->Add( bSizer5, 1, wxEXPAND, 5 );

	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );

	/*

	wxStaticLine * m_staticline = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );

	m_checkBox21 = new wxCheckBox( m_panel1, ID_USE_PROXY, _("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer2->Add( m_checkBox21, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText7 = new wxStaticText( m_panel1, wxID_ANY, _("    Адрес прокси-сервера:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer31->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl7 = new wxTextCtrl( m_panel1, ID_PROXY_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl7->SetMinSize( wxSize( 100,-1 ) );

	bSizer32->Add( m_textCtrl7, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText8 = new wxStaticText( m_panel1, wxID_ANY, _("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer32->Add( m_staticText8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl8 = new wxTextCtrl( m_panel1, ID_PROXY_PORT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl8->SetMinSize( wxSize( 60,-1 ) );

	bSizer32->Add( m_textCtrl8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer31->Add( bSizer32, 0, wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel1, wxID_ANY, _("    Имя пользователя:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer31->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl9 = new wxTextCtrl( m_panel1, ID_PROXY_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl9->SetMinSize( wxSize( 110,-1 ) );

	bSizer33->Add( m_textCtrl9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText10 = new wxStaticText( m_panel1, wxID_ANY, _("Пароль:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer33->Add( m_staticText10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl10 = new wxTextCtrl( m_panel1, ID_PROXY_PASS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_textCtrl10->SetMinSize( wxSize( 110,-1 ) );

	bSizer33->Add( m_textCtrl10, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	fgSizer31->Add( bSizer33, 1, wxEXPAND, 5 );

	bSizer2->Add( fgSizer31, 0, wxEXPAND, 5 );

	*/

	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_notebook->AddPage( m_panel1, _("Папки и файлы"), true );

	m_panel4 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	m_tools = new wxToolBar( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS );
	m_tools->AddTool( ID_APPEND_TYPE, _("Добавить"), wxNullBitmap);
	m_tools->AddTool( ID_MODIFY_TYPE, _("Изменить"), wxNullBitmap);
	m_tools->AddTool( ID_DELETE_TYPE, _("Удалить"), wxNullBitmap);
	m_tools->Realize();

	bSizer10->Add( m_tools, 0, wxALL|wxEXPAND, 5 );

	m_typelist = new TypeListCtrl( m_panel4, ID_TYPELIST, wxLC_REPORT|wxLC_VRULES|wxSUNKEN_BORDER );
    m_typelist->InsertColumn(0, _T("Тип"), wxLIST_FORMAT_LEFT, 50);
    m_typelist->InsertColumn(1, _T("Программа"), wxLIST_FORMAT_LEFT, 300);

	bSizer10->Add( m_typelist, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	m_panel4->SetSizer( bSizer10 );
	m_panel4->Layout();
	bSizer10->Fit( m_panel4 );
	m_notebook->AddPage( m_panel4, _("Типы файлов"), false );

	m_panel2 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, _("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer9->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_textCtrl6 = new wxTextCtrl( m_panel2, ID_EXTERNAL_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	bSizer9->Add( m_textCtrl6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton6 = new wxBitmapButton( m_panel2, ID_EXTERNAL_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer9->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer8->Add( bSizer9, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox2 = new wxCheckBox( m_panel2, ID_TRANSLIT_FOLDER, _("Транслитерация имени папки"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer10->Add( m_checkBox2, 0, wxALL, 5 );

	m_checkBox3 = new wxCheckBox( m_panel2, ID_TRANSLIT_FILE, _("Транслитерация имен файлов"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer10->Add( m_checkBox3, 0, wxALL, 5 );

	wxString m_radioBox1Choices[] = { _("Автор / Книга"), _("Автор / Серия / Номер_Книга") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( m_panel2, ID_FOLDER_FORMAT, _("Организация папок и файлов"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	fgSizer10->Add( m_radioBox1, 0, wxALL, 5 );

	wxString m_radioBox2Choices[] = { _("filename.fb2"), _("filename.fb2.zip") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_panel2, ID_FILE_FORMAT, _("Формат выгрузки"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox2->SetSelection( 0 );
	fgSizer10->Add( m_radioBox2, 0, wxALL, 5 );

	bSizer8->Add( fgSizer10, 0, wxEXPAND, 5 );

	m_panel2->SetSizer( bSizer8 );
	m_panel2->Layout();
	bSizer8->Fit( m_panel2 );
	m_notebook->AddPage( m_panel2, _("Внешнее устройство"), false );

	bSizerMain->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizerBtn = new wxStdDialogButtonSizer();
	m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	bSizerMain->Add( m_sdbSizerBtn, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
}

SettingsDlg::~SettingsDlg()
{
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
        {FB_LIBRARY_DIR, SettingsDlg::ID_LIBRARY_DIR_TXT, tText},
        {FB_WANRAIK_DIR, SettingsDlg::FB_WANRAIK_DIR_TXT, tText},
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
                if (wxTextCtrl * control = (wxTextCtrl*)FindWindowById(ids[i].control)) {
                    if (write)
                        params.SetText(ids[i].param, control->GetValue());
                    else
                        control->SetValue(params.GetText(ids[i].param));
				} break;
            case tCheck:
                if (wxCheckBox * control = (wxCheckBox*)FindWindowById(ids[i].control)) {
                    if (write)
                        params.SetValue(ids[i].param, control->GetValue());
                    else
                        control->SetValue((bool)params.GetValue(ids[i].param));
				} break;
            case tRadio:
                if (wxRadioBox * control = (wxRadioBox*)FindWindowById(ids[i].control)) {
                    if (write)
                        params.SetValue(ids[i].param, control->GetSelection());
                    else
                        control->SetSelection(params.GetValue(ids[i].param));
				} break;
        }

    }
};

void SettingsDlg::Execute(wxWindow* parent)
{
    SettingsDlg dlg(parent, wxID_ANY, _("Настройка параметров программы"), wxDefaultPosition, wxDefaultSize);

    dlg.Assign(false);
	dlg.FillTypelist();

    if (dlg.ShowModal() == wxID_OK) {
		dlg.Assign(true);
		dlg.SaveTypelist();
		ZipReader::Init();
    }
};

void SettingsDlg::FillTypelist()
{
	wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("\
		SELECT \
			books.file_type, types.command, \
			CASE WHEN books.file_type='fb2' THEN 1 ELSE 2 END AS number\
		FROM ( \
			 SELECT DISTINCT LOWER(file_type) AS file_type FROM BOOKS GROUP BY file_type \
			 UNION SELECT DISTINCT file_type FROM types \
			 UNION SELECT 'fb2' \
			 UNION SELECT 'pdf' \
			 UNION SELECT 'djvu' \
			 UNION SELECT 'txt' \
		) AS books \
		  LEFT JOIN types ON books.file_type = types.file_type \
		ORDER BY number, books.file_type \
     ");

	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	DatabaseResultSet* result = pStatement->ExecuteQuery();

	m_typelist->Freeze();

	long item = 0;
	while ( result && result->Next() ) {
		wxString file_type = result->GetResultString(wxT("file_type"));
		wxString command = result->GetResultString(wxT("command"));
		if (file_type == wxT("exe")) continue;

		if (command.IsEmpty())
			command = FbManager::GetSystemCommand(file_type);

		item = m_typelist->InsertItem(item + 1, file_type);
		m_typelist->SetItem(item, 1, command);
		m_typelist->SetItemData(item, m_commands.Add(command));
	}
	m_typelist->Thaw();
}

void SettingsDlg::OnTypelistActivated( wxListEvent & event )
{
	SelectApplication();
	event.Skip();
}

void SettingsDlg::SelectApplication()
{
	wxArrayTreeItemIds selections;
	size_t count = m_typelist->GetSelectedItemCount();

	if (!count) return;

	wxString title = _("Выберите приложение для просмотра файлов…");
	wxString command;
    long item = m_typelist->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if ( item >= 0) command = m_commands[m_typelist->GetItemData(item)];

#ifdef __WIN32__
    wxString wildCard = _("Исполняемые файлы (*.exe)|*.exe");

    wxFileDialog dlg (
		this,
		title,
		wxEmptyString,
		command,
		wildCard,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
		command = dlg.GetPath();
		size_t index = m_commands.Add(command);
		long item = -1;
		while (true) {
			item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (item == -1) break;
			m_typelist->SetItem(item, 1, command);
			m_typelist->SetItemData(item, index);
		}
    }

#else
	command = wxGetTextFromUser(title, _("Настройки:"), command);
	if (command.IsEmpty()) return;
	size_t index = m_commands.Add(command);
	item = -1;
    while (true) {
        item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1) break;
        m_typelist->SetItem(item, 1, command);
        m_typelist->SetItemData(item, index);
    }
#endif
}

void SettingsDlg::SaveTypelist()
{
	wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
	Types types(wxGetApp().GetDatabase());
	TypesRowSet * rows = types.All();

	AutoTransaction trans;

	for (size_t i = 0; i<rows->Count(); i++)
		rows->Item(i)->isOk = false;

	long item = -1;
	while (true) {
		item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (item == -1) break;
		wxString file_type = m_typelist->GetItemText(item);
		wxString command = m_commands[m_typelist->GetItemData(item)];
		bool found = false;
		for (size_t i = 0; i<rows->Count(); i++) {
			TypesRow * row = rows->Item(i);
			if (row->file_type == file_type) {
				found = true;
				row->isOk = true;
				if (command.IsEmpty()) {
					row->Delete();
				} else {
					wxString sys_command = FbManager::GetSystemCommand(file_type);
					if (sys_command == command) {
						row->Delete();
					} else {
						row->command = command;
						row->Save();
					}
				}
				break;
			}
		}
		if (found) continue;
		if (command.IsEmpty()) continue;

		wxString sys_command = FbManager::GetSystemCommand(file_type);
		if (sys_command == command) continue;

		TypesRow * row = types.New();
		row->file_type = file_type;
		row->command = command;
		row->Save();
		row->isOk = true;
	}

	for (size_t i = 0; i<rows->Count(); i++)
		if (! rows->Item(i)->isOk)
			rows->Item(i)->Delete();
}

void SettingsDlg::OnAppendType( wxCommandEvent& event )
{
	wxString filetype = wxGetTextFromUser(_("Введите новый тип файла:"), _("Настройки:"));
	if (filetype.IsEmpty()) return;
	filetype = filetype.Lower();

	long item = -1;
	while (true) {
		item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (item == -1) break;
		if (filetype == m_typelist->GetItemText(item)) return;
	}
	item = m_typelist->InsertItem(-1, filetype);
}

void SettingsDlg::OnModifyType( wxCommandEvent& event )
{
	SelectApplication();
}

void SettingsDlg::OnDeleteType( wxCommandEvent& event )
{
	m_typelist->Freeze();

	wxArrayInt items;

	long item = -1;
	while (true) {
		item = m_typelist->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (item == -1) break;
		m_typelist->DeleteItem(item);
		items.Add(item);
	}

	m_typelist->Thaw();
}


