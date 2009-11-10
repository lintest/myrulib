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
//		int w = GetClientSize().x - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) - 6;
		int w = GetClientSize().x;
		SetColumnWidth(1, 50);
		SetColumnWidth(1, w - 50);
	}
}

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( SettingsDlg, wxDialog )
	EVT_BUTTON( ID_DOWNLOAD_DIR_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTERNAL_BTN, SettingsDlg::OnSelectFolderClick )
	EVT_MENU( ID_APPEND_TYPE, SettingsDlg::OnAppendType )
	EVT_MENU( ID_MODIFY_TYPE, SettingsDlg::OnModifyType )
	EVT_MENU( ID_DELETE_TYPE, SettingsDlg::OnDeleteType )
	EVT_LIST_ITEM_ACTIVATED(ID_TYPELIST, SettingsDlg::OnTypelistActivated)
END_EVENT_TABLE()

SettingsDlg::SettingsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxDialog( parent, id, title, pos, size, style )
{

	wxFont font;
	wxString text = FbParams::GetText(FB_FONT_DLG);
	font.SetNativeFontInfoUserDesc(text);
	SetFont(font);

	m_database.AttachConfig();

	wxNotebook* m_notebook;
	wxPanel* panel;
	wxStaticText* m_staticText6;
	wxTextCtrl* m_textCtrl6;
	wxBitmapButton* m_bpButton6;
	wxCheckBox* m_checkBox2;
	wxCheckBox* m_checkBox3;
	wxRadioBox* m_radioBox1;
	wxRadioBox* m_radioBox2;
	wxToolBar* m_tools;
	wxStdDialogButtonSizer* m_sdbSizerBtn;
	wxButton* m_sdbSizerBtnOK;
	wxButton* m_sdbSizerBtnCancel;

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerFont;
	bSizerFont = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizerFont1;
	bSizerFont1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText * stFont1 = new wxStaticText( panel, wxID_ANY, wxT("Шрифт основного окна:"), wxDefaultPosition, wxDefaultSize, 0 );
	stFont1->Wrap( -1 );
	bSizerFont1->Add( stFont1, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxStaticText * stFont2 = new wxStaticText( panel, wxID_ANY, wxT("Шрифт диалогов:"), wxDefaultPosition, wxDefaultSize, 0 );
	stFont2->Wrap( -1 );
	bSizerFont1->Add( stFont2, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	bSizerFont->Add( bSizerFont1, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizerFont2;
	bSizerFont2 = new wxBoxSizer( wxVERTICAL );

	m_FontMain = new wxFontPickerCtrl( panel, ID_FONT_MAIN, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL );
	m_FontMain->SetMaxPointSize( 100 );
	bSizerFont2->Add( m_FontMain, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_FontDlg = new wxFontPickerCtrl( panel, ID_FONT_DLG, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL );
	m_FontDlg->SetMaxPointSize( 100 );
	bSizerFont2->Add( m_FontDlg, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizerFont->Add( bSizerFont2, 1, wxEXPAND, 5 );

	bSizer1->Add( bSizerFont, 0, wxEXPAND, 5 );

	panel->SetSizer( bSizer1 );
	panel->Layout();
	bSizer1->Fit( panel );

	m_notebook->AddPage( panel, _("Внешний вид"), true );

	panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxCheckBox * m_checkBox13 = new wxCheckBox( panel, ID_AUTO_DOWNLD, _("Автоматически стартовать загрузку файлов"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_checkBox13, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* m_staticText11 = new wxStaticText( panel, wxID_ANY, _("Адрес сайта Либрусек:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer12->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxComboBox * m_comboBox1 = new wxComboBox( panel, ID_LIBRUSEC_URL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox1->Append( _("http://lib.rus.ec") );
	m_comboBox1->Append( _("http://lib.ololo.cc") );
	bSizer12->Add( m_comboBox1, 1, wxALL, 5 );

	bSizer2->Add( bSizer12, 0, wxEXPAND|wxLEFT, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	wxCheckBox * m_checkBox12 = new wxCheckBox( panel, ID_USE_PROXY, _("Использовать прокси-сервер"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer13->Add( m_checkBox12, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxComboBox * m_comboBox2 = new wxComboBox( panel, ID_PROXY_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox2->Append( _("192.168.0.1:3128") );
	m_comboBox2->Append( _("172.16.0.1:3128") );
	m_comboBox2->Append( _("10.0.0.1:3128") );
	bSizer13->Add( m_comboBox2, 1, wxALL, 5 );

	bSizer2->Add( bSizer13, 0, wxEXPAND, 5 );

	m_staticText6 = new wxStaticText( panel, wxID_ANY, _("Папка для хранения скачиваемых файлов:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer2->Add( m_staticText6, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxBoxSizer* bSizer14 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl6 = new wxTextCtrl( panel, ID_DOWNLOAD_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	bSizer14->Add( m_textCtrl6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton6 = new wxBitmapButton( panel, ID_DOWNLOAD_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer14->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer2->Add( bSizer14, 0, wxEXPAND, 5 );

	panel->SetSizer( bSizer2 );
	panel->Layout();
	bSizer2->Fit( panel );
	m_notebook->AddPage( panel, _("Интернет"), false );

	panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	m_tools = new wxToolBar( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS );
	m_tools->AddTool( ID_APPEND_TYPE, _("Добавить"), wxNullBitmap);
	m_tools->AddTool( ID_MODIFY_TYPE, _("Изменить"), wxNullBitmap);
	m_tools->AddTool( ID_DELETE_TYPE, _("Удалить"), wxNullBitmap);
	m_tools->Realize();

	bSizer10->Add( m_tools, 0, wxALL|wxEXPAND, 5 );

	m_typelist = new TypeListCtrl( panel, ID_TYPELIST, wxLC_REPORT|wxLC_VRULES|wxSUNKEN_BORDER );
	m_typelist->InsertColumn(0, _T("Тип"), wxLIST_FORMAT_LEFT, 50);
	m_typelist->InsertColumn(1, _T("Программа"), wxLIST_FORMAT_LEFT, 300);

	bSizer10->Add( m_typelist, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	panel->SetSizer( bSizer10 );
	panel->Layout();
	bSizer10->Fit( panel );
	m_notebook->AddPage( panel, _("Типы файлов"), false );

	panel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( panel, wxID_ANY, _("Папка внешнего\nустройства:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer9->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_textCtrl6 = new wxTextCtrl( panel, ID_EXTERNAL_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl6->SetMinSize( wxSize( 300,-1 ) );

	bSizer9->Add( m_textCtrl6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bpButton6 = new wxBitmapButton( panel, ID_EXTERNAL_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer9->Add( m_bpButton6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	bSizer8->Add( bSizer9, 0, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBox2 = new wxCheckBox( panel, ID_TRANSLIT_FOLDER, _("Транслитерация имени папки"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_checkBox2, 0, wxALL, 5 );

	m_checkBox3 = new wxCheckBox( panel, ID_TRANSLIT_FILE, _("Транслитерация имен файлов"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_checkBox3, 0, wxALL, 5 );

	wxString m_radioBox1Choices[] = {
		_("А / Автор / Серия / №_Книга"),
		_("А / Автор / Книга"),
		_("Автор / Серия / №_Книга"),
		_("Автор / Книга"),
	};
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( panel, ID_FOLDER_FORMAT, _("Организация папок и файлов"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	fgSizer10->Add( m_radioBox1, 0, wxALL, 5 );

	wxString m_radioBox2Choices[] = { _("filename.fb2"), _("filename.fb2.zip") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( panel, ID_FILE_FORMAT, _("Формат выгрузки"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox2->SetSelection( 0 );
	fgSizer10->Add( m_radioBox2, 0, wxALL, 5 );

	bSizer8->Add( fgSizer10, 0, wxEXPAND, 5 );
/*
	m_checkBox2 = new wxCheckBox( panel, ID_USE_SYMLINKS, _("Создавть cимвольные ссылки для соавторов"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_checkBox2, 0, wxALL, 5 );
*/
	panel->SetSizer( bSizer8 );
	panel->Layout();
	bSizer8->Fit( panel );
	m_notebook->AddPage( panel, _("Внешнее устройство"), false );

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
		tCombo,
		tFont,
	};
	struct Struct{
		int param;
		ID control;
		Type type;
	};

	const Struct ids[] = {
		{FB_AUTO_DOWNLD, SettingsDlg::ID_AUTO_DOWNLD, tCheck},
		{FB_USE_PROXY, SettingsDlg::ID_USE_PROXY, tCheck},
		{FB_PROXY_ADDR, SettingsDlg::ID_PROXY_ADDR, tCombo},
		{FB_LIBRUSEC_URL, SettingsDlg::ID_LIBRUSEC_URL, tCombo},
		{FB_DOWNLOAD_DIR, SettingsDlg::ID_DOWNLOAD_DIR_TXT, tText},
		{FB_EXTERNAL_DIR, SettingsDlg::ID_EXTERNAL_TXT, tText},
		{FB_TRANSLIT_FOLDER, SettingsDlg::ID_TRANSLIT_FOLDER, tCheck},
		{FB_TRANSLIT_FILE, SettingsDlg::ID_TRANSLIT_FILE, tCheck},
		{FB_FOLDER_FORMAT, SettingsDlg::ID_FOLDER_FORMAT, tRadio},
		{FB_FILE_FORMAT, SettingsDlg::ID_FILE_FORMAT, tRadio},
		{FB_FONT_MAIN, SettingsDlg::ID_FONT_MAIN, tFont},
		{FB_FONT_DLG, SettingsDlg::ID_FONT_DLG, tFont},
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
						control->SetValue(params.GetValue(ids[i].param) != 0);
				} break;
			case tRadio:
				if (wxRadioBox * control = (wxRadioBox*)FindWindowById(ids[i].control)) {
					if (write)
						params.SetValue(ids[i].param, control->GetSelection());
					else
						control->SetSelection(params.GetValue(ids[i].param));
				} break;
			case tCombo:
				if (wxComboBox * control = (wxComboBox*)FindWindowById(ids[i].control)) {
					if (write)
						params.SetText(ids[i].param, control->GetValue());
					else
						control->SetValue(params.GetText(ids[i].param));
				} break;
			case tFont:
				if (wxFontPickerCtrl * control = (wxFontPickerCtrl*)FindWindowById(ids[i].control)) {
					if (write) {
						wxFont font = control->GetSelectedFont();
						wxString text = font.GetNativeFontInfoUserDesc();
						params.SetText(ids[i].param, text);
					} else {
						wxFont font;
						wxString text = params.GetText(ids[i].param);;
						font.SetNativeFontInfoUserDesc(text);
						control->SetSelectedFont(font);
					}
				} break;
		}

	}

	if (write) {
		wxGetApp().GetTopWindow()->SetFont(m_FontMain->GetSelectedFont());
	}
};

void SettingsDlg::Execute(wxWindow* parent)
{
	SettingsDlg dlg(parent, wxID_ANY, _("Настройка параметров программы"), wxDefaultPosition, wxDefaultSize);

	try {
		dlg.Assign(false);
		dlg.FillTypelist();
	} catch (wxSQLite3Exception & e) {
		wxLogFatalError(e.GetMessage());
		return;
	}

	if (dlg.ShowModal() == wxID_OK) {
		dlg.Assign(true);
		dlg.SaveTypelist();
		ZipReader::Init();
	}
};

void SettingsDlg::FillTypelist()
{
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
		  LEFT JOIN config.types as types ON books.file_type = types.file_type \
		ORDER BY number, books.file_type \
	 ");

	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);

	m_typelist->Freeze();

	long item = 0;
	while ( result.NextRow() ) {
		wxString file_type = result.GetString(wxT("file_type"));
		wxString command = result.GetString(wxT("command"));
		if (file_type.IsEmpty()) continue;
		if (file_type == wxT("exe")) continue;
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
	FbLocalDatabase database;
	FbAutoCommit transaction(m_database);

	for (size_t i=0; i<m_deleted.Count(); i++) {
		wxString sql = wxT("DELETE FROM types WHERE file_type=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_deleted[i]);
		stmt.ExecuteUpdate();
	}

	long item = -1;
	while (true) {
		item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL);
		if (item == wxNOT_FOUND) break;
		wxString file_type = m_typelist->GetItemText(item);
		wxString command = m_commands[m_typelist->GetItemData(item)];
		if ( command.IsEmpty() ) {
			wxString sql = wxT("DELETE FROM types WHERE file_type=?");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, file_type);
			stmt.ExecuteUpdate();
		} else {
			wxString sql = wxT("INSERT OR REPLACE INTO types(file_type, command) values(?,?)");
			wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
			stmt.Bind(1, file_type);
			stmt.Bind(2, command);
			stmt.ExecuteUpdate();
		}
	}
}

void SettingsDlg::OnAppendType( wxCommandEvent& event )
{
	wxString filetype = wxGetTextFromUser(_("Введите новый тип файла:"), _("Настройки:"));
	if (filetype.IsEmpty()) return;
	filetype = filetype.Lower();

	long item = -1;
	bool bExists = false;
	long stateMask = wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED;
	while (true) {
		item = m_typelist->GetNextItem(item, wxLIST_NEXT_ALL);
		if (item == wxNOT_FOUND) break;
		long state = 0;
		if (m_typelist->GetItemText(item) == filetype) {
			state = wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED;
			bExists = true;
		}
		m_typelist->SetItemState(item, state, stateMask);
	}

	if (bExists) return;

	item = m_typelist->InsertItem(0, filetype);
	m_typelist->SetItemState(item, stateMask, stateMask);
}

void SettingsDlg::OnModifyType( wxCommandEvent& event )
{
	SelectApplication();
}

void SettingsDlg::OnDeleteType( wxCommandEvent& event )
{
	m_typelist->Freeze();

	long item = -1;
	while (true) {
		item = m_typelist->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (item == wxNOT_FOUND) break;
		m_deleted.Add(m_typelist->GetItemText(item));
		m_typelist->DeleteItem(item);
	}

	m_typelist->Thaw();
}

