#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/listbook.h>
#include <wx/aui/auibar.h>
#include "FbParams.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbParamsDlg.h"
#include "MyRuLibApp.h"
#include "FbViewerDlg.h"
#include "FbLocale.h"
#include "FbString.h"
#include "FbDatabase.h"
#include "FbFileReader.h"
#include "controls/FbChoiceCtrl.h"
#include "controls/FbComboBox.h"
#include "controls/FbToolBar.h"
#include "controls/FbTreeView.h"
#include "FbLogoBitmap.h"

static const char * blank_xpm[] = {
"1 1 2 1",
"X c Red",
"  c None",
" "};

//-----------------------------------------------------------------------------
//  FbParamsDlg::LoadThread
//-----------------------------------------------------------------------------

void * FbParamsDlg::LoadThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);
	database.AttachConfig();
	LoadScripts(database);
	LoadTypes(database);
	return NULL;
}

void FbParamsDlg::LoadThread::LoadTypes(wxSQLite3Database &database)
{
	wxString sql = GetCommandSQL(wxT("config"));
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (!result.IsOk()) return;
	FbListStore * model = new FbListStore;
	while ( result.NextRow() ) {
		wxString type = result.GetString(0);
		if (type.IsEmpty() || type == wxT("exe")) continue;
		model->Append(new TypeData(result));
	}
	FbModelEvent event(ID_TYPE_LIST, model);
	m_frame->AddPendingEvent(event);
}

void FbParamsDlg::LoadThread::LoadScripts(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT id, name, text FROM script ORDER BY id");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (!result.IsOk()) return;
	FbListStore * model = new FbListStore;
	while ( result.NextRow() ) {
		model->Append(new ScriptData(result));
	}
	FbModelEvent(ID_SCRIPT_LIST, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::TypeData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbParamsDlg::TypeData, FbModelData)

FbParamsDlg::TypeData::TypeData(wxSQLite3ResultSet &result)
	: m_type(result.GetString(0)), m_command(result.GetString(1)), m_modified(false)
{
}

wxString FbParamsDlg::TypeData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0: return m_type;
		case 1: return m_command;
		case 2: return (m_command == wxT('*')) ? fbT("~CR3") : m_command;
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::ScriptData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbParamsDlg::ScriptData, FbModelData)

FbParamsDlg::ScriptData::ScriptData(wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_text(result.GetString(2)), m_modified(false)
{
}

wxString FbParamsDlg::ScriptData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0: return m_name;
		case 1: return m_text;
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::ScriptDlg
//-----------------------------------------------------------------------------

FbParamsDlg::ScriptDlg::ScriptDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: FbDialog(parent, id, title, pos, size, style)
{
	m_letters = wxT("fpnde");
	const wxChar * helps[] = {
		_("Full file path with name and extension"),
		_("Full file path without extension"),
		_("File name without path and extension"),
		_("Target directory"),
		_("File name extension"),
	};

	wxAuiToolBar * toolbar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT );
	toolbar->SetToolBitmapSize(wxSize(1, 1));
	size_t length = m_letters.Length();
	for (size_t i = 0; i < length; i++) {
		int btnid = ID_LETTERS + i;
		wxString title = (wxString)wxT('%') << m_letters[i];
		toolbar->AddTool(btnid, title, wxBitmap(blank_xpm), (wxString)helps[i]);
		this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ScriptDlg::OnLetterClicked));
	}
	toolbar->Realize();

	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerName = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Extension"));
	info->Wrap( -1 );
	bSizerName->Add( info, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_name.Create( this, wxID_ANY);
	bSizerName->Add( &m_name, 1, wxALL, 5 );

	bSizerMain->Add( bSizerName, 0, wxEXPAND, 5 );

	m_text.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_DONTWRAP );
	bSizerMain->Add( &m_text, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5 );
	bSizerMain->SetMinSize(-1, 200);

	wxBoxSizer* bSizerButtons = new wxBoxSizer( wxHORIZONTAL );

	bSizerButtons->Add( toolbar, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerButtons->Add( sdbSizerBtn, 1, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	bSizerMain->Add( bSizerButtons, 0, wxEXPAND, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());

	m_name.SetFocus();
}

void FbParamsDlg::ScriptDlg::OnLetterClicked(wxCommandEvent& event)
{
	int pos = event.GetId() - ID_LETTERS;
	if (0 <= pos && pos < (int)m_letters.Length()) {
		wxKeyEvent event(wxEVT_CHAR);
		event.m_keyCode = wxT('%');
		m_text.EmulateKeyPress(event);
		event.m_keyCode = m_letters[pos];
		m_text.EmulateKeyPress(event);
	}
}

bool FbParamsDlg::ScriptDlg::Execute(wxWindow* parent, const wxString& title, wxString &name, wxString &text)
{
	ScriptDlg dlg(parent, wxID_ANY, title);
	dlg.m_name.SetValue(name);
	dlg.m_text.SetValue(text);
	if (dlg.ShowModal() == wxID_OK) {
		name = dlg.m_name.GetValue();
		text = dlg.m_text.GetValue();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelFont
//-----------------------------------------------------------------------------

FbParamsDlg::PanelFont::PanelFont(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer * fgSizerList = new wxFlexGridSizer( 2 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	AppendItem(fgSizerList, _("List of authors and books:"), ID_FONT_MAIN, ID_COLOUR_MAIN);
	AppendItem(fgSizerList, _("Toolbar:"), ID_FONT_TOOL, ID_COLOUR_TOOL);
	AppendItem(fgSizerList, _("Information:"), ID_FONT_HTML, ID_COLOUR_HTML);
	AppendItem(fgSizerList, _("Dialog windows:"), ID_FONT_DLG, ID_COLOUR_DLG);

	bSizerMain->Add( fgSizerList, 0, wxEXPAND, 5 );

	wxButton * btnClear = new wxButton( this, ID_FONT_CLEAR, _("Reset fonts settings"));
	bSizerMain->Add( btnClear, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

void FbParamsDlg::PanelFont::AppendItem(wxFlexGridSizer* fgSizer, const wxString& name, wxWindowID idFont, wxWindowID idColour)
{
	wxStaticText * stTitle;
	stTitle = new wxStaticText( this, wxID_ANY, name);
	stTitle->Wrap( -1 );
	fgSizer->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFontPickerCtrl * fpValue;
	fpValue = new wxFontPickerCtrl( this, idFont, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL );
	fpValue->SetMaxPointSize( 100 );
	fgSizer->Add( fpValue, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
/*
	wxColourPickerCtrl * cpValue;
	cpValue = new wxColourPickerCtrl( this, idColour);
	fgSizer->Add( cpValue, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
*/
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelInternet
//-----------------------------------------------------------------------------

FbParamsDlg::PanelInternet::PanelInternet(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxCheckBox * checkbox = new wxCheckBox( this, ID_AUTO_DOWNLD, _("Automatically begin files downloading"));
	bSizerMain->Add( checkbox, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer * bSizerProxy = new wxBoxSizer( wxHORIZONTAL );

	checkbox = new wxCheckBox( this, ID_USE_PROXY, _("Use proxy-server") );
	bSizerProxy->Add( checkbox, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxComboBox * combobox = new wxComboBox( this, ID_PROXY_ADDR );
	combobox->Append( wxT("192.168.0.1:3128") );
	combobox->Append( wxT("172.16.0.1:3128") );
	combobox->Append( wxT("10.0.0.1:3128") );
	bSizerProxy->Add( combobox, 1, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( bSizerProxy, 0, wxEXPAND, 5 );

	wxFlexGridSizer * fSizerProxy = new wxFlexGridSizer( 2 );
	fSizerProxy->AddGrowableCol( 1 );
	fSizerProxy->SetFlexibleDirection( wxBOTH );
	fSizerProxy->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * textUser = new wxStaticText( this, wxID_ANY, _("User name:") );
	textUser->Wrap( -1 );
	fSizerProxy->Add( textUser, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 20 );

	wxTextCtrl * editUser = new wxTextCtrl( this, ID_PROXY_USER );
	fSizerProxy->Add( editUser, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * textPass = new wxStaticText( this, wxID_ANY, _("Password:") );
	textPass->Wrap( -1 );
	fSizerProxy->Add( textPass, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 20 );

	wxTextCtrl * editPass = new wxTextCtrl( this, ID_PROXY_PASS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	fSizerProxy->Add( editPass, 0, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( fSizerProxy, 0, wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_HTTP_IMAGES, _("Load images for author's description"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	wxStaticText * text = new wxStaticText( this, wxID_ANY, _("Folder to save downloads:"));
	text->Wrap( -1 );
	bSizerMain->Add( text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	FbCustomCombo * combo = new FbCustomCombo( this, ID_DOWNLOAD_DIR);
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_DEL_DOWNLOAD, _("Delete downloaded files when download query removed"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	wxFlexGridSizer * fgSizer = new wxFlexGridSizer( 2 );
	fgSizer->SetFlexibleDirection( wxBOTH );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	text = new wxStaticText( this, wxID_ANY, _("Download timout (seconds):"));
	text->Wrap( -1 );
	fgSizer->Add( text, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxSpinCtrl * number = new wxSpinCtrl( this, ID_WEB_TIMEOUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer->Add( number, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	text = new wxStaticText( this, wxID_ANY, _("Number of attempts:"));
	text->Wrap( -1 );
	fgSizer->Add( text, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	number = new wxSpinCtrl( this, ID_WEB_ATTEMPT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 9, 0 );
	fgSizer->Add( number, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	bSizerMain->Add( fgSizer, 0, wxEXPAND, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelTypes
//-----------------------------------------------------------------------------

FbParamsDlg::PanelTypes::PanelTypes(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = new wxToolBar( this, ID_TYPE_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	toolbar->AddTool( ID_APPEND_TYPE, _("Append"), wxBitmap(add_xpm))->Enable(false);
	toolbar->AddTool( ID_MODIFY_TYPE, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	toolbar->AddTool( ID_DELETE_TYPE, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	toolbar->Realize();
	bSizerMain->Add( toolbar, 0, wxALL|wxEXPAND, 5 );

	FbTreeViewCtrl * treeview = new FbTreeViewCtrl( this, ID_TYPE_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	treeview->AddColumn(0, _("Extension"), 6);
	treeview->AddColumn(2, _("Program"), -10);
	bSizerMain->Add( treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	#ifdef FB_INCLUDE_READER
	wxCheckBox * checkbox = new wxCheckBox( this, ID_USE_COOLREADER, _("Use builtin CoolReader3"));
	bSizerMain->Add( checkbox, 0, wxEXPAND|wxALL, 5 );
	#endif // FB_INCLUDE_READER

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelInterface
//-----------------------------------------------------------------------------

FbParamsDlg::PanelInterface::PanelInterface(wxWindow *parent)
	:wxPanel(parent)
{
	wxStaticText * text;
	wxCheckBox * checkbox;
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerLocale = new wxBoxSizer( wxHORIZONTAL );

	text = new wxStaticText( this, wxID_ANY, _("Language"));
	text->Wrap( -1 );
	bSizerLocale->Add( text, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	FbChoiceInt * localeChoice = new FbChoiceInt( this, ID_LANG_LOCALE);
	FbLocale::Fill(localeChoice, FbParams(FB_LANG_LOCALE));
	bSizerLocale->Add( localeChoice, 1, wxALL, 5 );

	bSizerMain->Add(bSizerLocale, 0, wxEXPAND);

	checkbox = new wxCheckBox( this, ID_SAVE_FULLPATH, _("Save full path of the file when importing"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	text = new wxStaticText( this, wxID_ANY, _("Temporary folder:"));
	text->Wrap( -1 );
	bSizerMain->Add( text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	FbCustomCombo * combo = new FbCustomCombo( this, ID_TEMP_DIR);
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	#ifdef __WXGTK__
	wxBoxSizer* bSizerDir = new wxBoxSizer( wxHORIZONTAL );

	text = new wxStaticText( this, wxID_ANY, _("Wine temp folder:"));
	text->Wrap( -1 );
	bSizerDir->Add( text, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl * edit = new wxTextCtrl( this, ID_WINE_DIR);
	edit->SetMinSize( wxSize( 200,-1 ) );
	bSizerDir->Add( edit, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerDir, 0, wxEXPAND, 5 );
	#endif

	checkbox = new wxCheckBox( this, ID_TEMP_DEL, _("Delete temporary files when you exit the program"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_REMOVE_FILES, _("Delete files when you delete a book"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_CLEAR_LOG, _("Clear the log window when it closes"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_GRAY_FONT, _("Use a gray font for missing books"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_GRID_HRULES, _("Draws light horizontal rules between rows"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_GRID_VRULES, _("Draws light vertical rules between columns"));
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	wxSpinCtrl * number;

	wxFlexGridSizer * fgSizer = new wxFlexGridSizer( 2 );
	fgSizer->SetFlexibleDirection( wxBOTH );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	text = new wxStaticText( this, wxID_ANY, _("Maximum length of file information"));
	text->Wrap( -1 );
	fgSizer->Add( text, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	number = new wxSpinCtrl( this, ID_FILE_LENGTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 16, 9999, 0 );
	fgSizer->Add( number, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	text = new wxStaticText( this, wxID_ANY, _("Maximum width of the cover image"));
	text->Wrap( -1 );
	fgSizer->Add( text, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	number = new wxSpinCtrl( this, ID_IMAGE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 50, 999, 0 );
	fgSizer->Add( number, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	bSizerMain->Add( fgSizer);

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelExport
//-----------------------------------------------------------------------------

FbParamsDlg::PanelExport::PanelExport(wxWindow *parent, wxString &letters)
	:wxPanel(parent)
{
	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticText * m_staticText6 = new wxStaticText( this, wxID_ANY, _("Destination folder:"));
	m_staticText6->Wrap( -1 );
	bSizerMain->Add( m_staticText6, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	FbCustomCombo * combo = new FbCustomCombo( this, ID_EXTERNAL_DIR);
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * format_info = new wxStaticText( this, wxID_ANY, _("Exported collection structure"));
	format_info->Wrap( -1 );
	bSizerMain->Add( format_info, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxComboBox * combobox = new wxComboBox( this, ID_FOLDER_FORMAT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	combobox->Append( wxT("%a/%f/%s/%n %t") );
	combobox->Append( wxT("%a/%f/%t") );
	combobox->Append( wxT("%f/%s/%n %t") );
	combobox->Append( wxT("%f/%s/%t") );
	combobox->Append( wxT("%f/%m") );
	bSizerMain->Add( combobox, 0, wxALL|wxEXPAND, 5 );

	letters = wxT("afcsnitlme");
	const wxChar * helps[] = {
		_("First letter of author name"),
		_("Full author name"),
		_("Short-cut author name"),
		_("Sequence name"),
		_("Number in sequence"),
		_("Internal book code"),
		_("Book title"),
		_("Book language"),
		_("Book file MD5 sum"),
		_("File name extension"),
	};

	wxAuiToolBar * toolbar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_TEXT );
	toolbar->SetToolBitmapSize(wxSize(1, 1));
	size_t length = letters.Length();
	for (size_t i = 0; i < length; i++) {
		int btnid = ID_LETTERS + i;
		wxString title = (wxString)wxT('%') << letters[i];
		toolbar->AddTool(btnid, title, wxBitmap(blank_xpm), (wxString)helps[i]);
		this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(FbParamsDlg::OnLetterClicked));
	}
	toolbar->Realize();
	bSizerMain->Add(toolbar, 0, wxEXPAND|wxALL, 5);

	wxCheckBox * checkbox;

	checkbox = new wxCheckBox( this, ID_TRANSLIT_FOLDER, _("Transliterate folder name"));
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_TRANSLIT_FILE, _("Transliterate filename"));
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_USE_UNDERSCORE, _("Replace spaces by underscores"));
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerNumber = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * numbText = new wxStaticText( this, wxID_ANY, _("Number of digits in the number"));
	numbText->Wrap( -1 );
	bSizerNumber->Add( numbText, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	FbChoiceInt * numbChoice = new FbChoiceInt( this, ID_NUMBER_FORMAT);
	wxString zero;
	for (size_t i = 1; i < 10; i++ ) {
		wxString text = zero;
		numbChoice->Append(text << i, i);
		zero << wxT('0');
	}
	numbChoice->SetSelection(0);
	bSizerNumber->Add( numbChoice, 1, wxALL, 5 );

	bSizerMain->Add(bSizerNumber, 0, wxEXPAND);
	wxBoxSizer* bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * typeText = new wxStaticText( this, wxID_ANY, _("Format of exporting collection"));
	typeText->Wrap( -1 );
	bSizerFormat->Add( typeText, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	wxString filename = _("filename");
	FbChoiceInt * typeChoice = new FbChoiceInt( this, ID_FILE_FORMAT);
	typeChoice->Append(filename << wxT(".fb2"), 0);
	typeChoice->Append(filename + wxT(".zip"), -1);
	typeChoice->Append(filename + wxT(".gz"),  -2);
	typeChoice->Append(filename + wxT(".bz2"), -3);
	typeChoice->SetSelection(0);
	bSizerFormat->Add( typeChoice, 1, wxALL, 5 );

	bSizerMain->Add(bSizerFormat, 0, wxEXPAND);

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelScripts
//-----------------------------------------------------------------------------

FbParamsDlg::PanelScripts::PanelScripts(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = new wxToolBar( this, ID_SCRIPT_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	toolbar->AddTool( ID_APPEND_SCRIPT, _("Append"), wxBitmap(add_xpm))->Enable(false);
	toolbar->AddTool( ID_MODIFY_SCRIPT, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	toolbar->AddTool( ID_DELETE_SCRIPT, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	toolbar->Realize();
	bSizerMain->Add( toolbar, 0, wxALL|wxEXPAND, 5 );

	FbTreeViewCtrl * treeview = new FbTreeViewCtrl( this, ID_SCRIPT_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	treeview->AddColumn(0, _("Extension"), 6);
	treeview->AddColumn(1, _("Export script"), -10);
	bSizerMain->Add( treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

//-----------------------------------------------------------------------------
//  FbParamsDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbParamsDlg, wxDialog )
	EVT_BUTTON( ID_TEMP_DIR, FbParamsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_DOWNLOAD_DIR, FbParamsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_EXTERNAL_DIR, FbParamsDlg::OnSelectFolderClick )
	EVT_BUTTON( ID_FONT_CLEAR, FbParamsDlg::OnFontClear )
	EVT_TOOL( ID_APPEND_TYPE, FbParamsDlg::OnAppendType )
	EVT_TOOL( ID_MODIFY_TYPE, FbParamsDlg::OnModifyType )
	EVT_TOOL( ID_DELETE_TYPE, FbParamsDlg::OnDeleteType )
	EVT_TOOL( ID_APPEND_SCRIPT, FbParamsDlg::OnAppendScript )
	EVT_TOOL( ID_MODIFY_SCRIPT, FbParamsDlg::OnModifyScript )
	EVT_TOOL( ID_DELETE_SCRIPT, FbParamsDlg::OnDeleteScript )
	EVT_TREE_ITEM_ACTIVATED(ID_TYPE_LIST, FbParamsDlg::OnTypeActivated)
	EVT_TREE_ITEM_ACTIVATED(ID_SCRIPT_LIST, FbParamsDlg::OnScriptActivated)
	EVT_FB_MODEL(ID_TYPE_LIST, FbParamsDlg::OnModel)
	EVT_FB_MODEL(ID_SCRIPT_LIST, FbParamsDlg::OnModel)
END_EVENT_TABLE()

FbParamsDlg::FbParamsDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style ), m_thread(this)
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxNotebook * notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	notebook->AddPage( new PanelInterface(notebook), _("General"), true );
	notebook->AddPage( new PanelInternet(notebook), _("Network"), false );
	notebook->AddPage( new PanelTypes(notebook), _("File types"), false );
	notebook->AddPage( new PanelExport(notebook, m_letters), _("Export"), false );
	notebook->AddPage( new PanelScripts(notebook), _("Scripts"), false );
	notebook->AddPage( new PanelFont(notebook), _("Fonts"), false );

	bSizerMain->Add( notebook, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());

	if (m_thread.Create() == wxTHREAD_NO_ERROR) m_thread.Run();
}

FbParamsDlg::~FbParamsDlg()
{
	m_thread.Close();
	m_thread.Wait();
}

void FbParamsDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxComboCtrl * control = wxDynamicCast(FindWindow(event.GetId()), wxComboCtrl);
	if (!control) return;

	wxDirDialog dlg(
		this,
		_("Select folder"),
		control->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK)  control->SetValue(dlg.GetPath());
}

void FbParamsDlg::Assign(bool write)
{
	struct Struct{
		int param;
		ID control;
	};

	const Struct ids[] = {
		{FB_AUTO_DOWNLD, ID_AUTO_DOWNLD},
		{FB_USE_PROXY, ID_USE_PROXY},
		{FB_PROXY_ADDR, ID_PROXY_ADDR},
		{FB_PROXY_USER, ID_PROXY_USER},
		{FB_PROXY_PASS, ID_PROXY_PASS},
		{FB_TEMP_DEL, ID_TEMP_DEL},
		{FB_TEMP_DIR, ID_TEMP_DIR},
		#ifdef __WXGTK__
		{FB_WINE_DIR, ID_WINE_DIR},
		#endif // __WXGTK__
		{FB_DOWNLOAD_DIR, ID_DOWNLOAD_DIR},
		{FB_DEL_DOWNLOAD, ID_DEL_DOWNLOAD},
		{FB_EXTERNAL_DIR, ID_EXTERNAL_DIR},
		{FB_TRANSLIT_FOLDER, ID_TRANSLIT_FOLDER},
		{FB_TRANSLIT_FILE, ID_TRANSLIT_FILE},
		{FB_USE_UNDERSCORE, ID_USE_UNDERSCORE},
		{FB_FOLDER_FORMAT, ID_FOLDER_FORMAT},
		{FB_NUMBER_FORMAT, ID_NUMBER_FORMAT},
		{FB_FONT_MAIN, ID_FONT_MAIN},
		{FB_FONT_HTML, ID_FONT_HTML},
		{FB_FONT_TOOL, ID_FONT_TOOL},
		{FB_FONT_DLG, ID_FONT_DLG},
		{FB_HTTP_IMAGES, ID_HTTP_IMAGES},
		{FB_REMOVE_FILES, ID_REMOVE_FILES},
		{FB_SAVE_FULLPATH, ID_SAVE_FULLPATH},
		{FB_CLEAR_LOG, ID_CLEAR_LOG},
		{FB_GRAY_FONT, ID_GRAY_FONT},
		{FB_FILE_FORMAT, ID_FILE_FORMAT},
		{FB_LANG_LOCALE, ID_LANG_LOCALE},
		{FB_WEB_TIMEOUT, ID_WEB_TIMEOUT},
		{FB_WEB_ATTEMPT, ID_WEB_ATTEMPT},
		{FB_FILE_LENGTH, ID_FILE_LENGTH},
		{FB_IMAGE_WIDTH, ID_IMAGE_WIDTH},
		{FB_GRID_HRULES, ID_GRID_HRULES},
		{FB_GRID_VRULES, ID_GRID_VRULES},
		#ifdef FB_INCLUDE_READER
		{FB_USE_COOLREADER, ID_USE_COOLREADER},
		#endif // FB_INCLUDE_READER
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	for (size_t i=0; i<idsCount; i++) {
		FbDialog::Assign(ids[i].control, ids[i].param, write);
	}

	if (write) FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATE_FONTS).Post();
};

void FbParamsDlg::Execute(wxWindow* parent)
{
	FbParamsDlg dlg(parent, wxID_ANY, _("Program settings"), wxDefaultPosition, wxDefaultSize);
	dlg.Assign(false);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) {
		dlg.Assign(true);
		dlg.SaveData();
		FbTempEraser::sm_erase = FbParams(FB_TEMP_DEL);
		wxGetApp().Localize();
	}
};

void FbParamsDlg::OnAppendScript( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_SCRIPT_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	wxString name, text;
	bool ok = ScriptDlg::Execute(this, _("Append export script"), name, text);
	if (!ok) return;

	int code = FbLocalDatabase().NewId(FB_NEW_SCRIPT);
	treeview->Append(new ScriptData(code, name, text));
	EnableTool(ID_SCRIPT_LIST, true);
	treeview->SetFocus();

	wxString label = _("filename"); label << wxT('.') << name;
	FbChoiceInt * typelist = wxDynamicCast(FindWindow(ID_FILE_FORMAT), FbChoiceInt);
	if (typelist) typelist->Append(label, code);
}

void FbParamsDlg::OnModifyScript( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_SCRIPT_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	ScriptData * data = wxDynamicCast(&item, ScriptData);
	if (!data) return;

	wxString name = data->GetValue(*model, 0);
	wxString text = data->GetValue(*model, 1);
	bool ok = ScriptDlg::Execute(this, _("Modify export script"), name, text);
	if (!ok) return;

	int code = data->GetCode();
	treeview->Replace(new ScriptData(code, name, text));
	treeview->SetFocus();

	FbChoiceInt * typelist = wxDynamicCast(FindWindow(ID_FILE_FORMAT), FbChoiceInt);
	if (typelist) {
		int index = typelist->GetSelection();
		size_t count = typelist->GetCount();
		for (size_t i = 0; i < count; i++) {
			int format = typelist->GetClientData(i);
			if (format == code) {
				wxString label = _("filename"); label << wxT('.') << name;
				typelist->SetString(i, label);
				break;
			}
		}
		typelist->SetSelection(index);
	}
}

void FbParamsDlg::OnDeleteScript( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_SCRIPT_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	ScriptData * data = wxDynamicCast(&item, ScriptData);
	if (!data) return;

	wxString msg = _("Delete export script") + COLON + data->GetValue(*model, 0);
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (!ok) return;

	int code = data->GetCode();
	if (code) m_del_scr.Add(data->GetCode());
	treeview->Delete();

	EnableTool(ID_SCRIPT_LIST, model->GetRowCount());
	treeview->SetFocus();

	FbChoiceInt * typelist = wxDynamicCast(FindWindow(ID_FILE_FORMAT), FbChoiceInt);
	if (typelist) {
		size_t index = (size_t) typelist->GetSelection();
		size_t count = typelist->GetCount();
		for (size_t i = 0; i < count; i++) {
			int format = typelist->GetClientData(i);
			if (format == code) {
				typelist->Delete(i);
				if (i == index) typelist->SetSelection(0);
				break;
			}
		}
	}
}

void FbParamsDlg::OnAppendType( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_TYPE_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	wxString filetype = wxGetTextFromUser(_("Input new filetype"), _("Settings"));
	filetype = filetype.Trim(false).Trim(true).Lower();
	if (filetype.IsEmpty()) return;

	size_t count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		TypeData * data = wxDynamicCast(&item, TypeData);
		if (data && data->GetValue(*model, 0) == filetype) {
			model->FindRow(i, true);
			return;
		}
	}

	treeview->Append(new TypeData(filetype));
	EnableTool(ID_TYPE_LIST, true);
	treeview->SetFocus();
}

void FbParamsDlg::OnModifyType( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_TYPE_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	TypeData * data = wxDynamicCast(&item, TypeData);
	if (!data) return;

	wxString title = _("Select the application to view files");
	wxString type = data->GetValue(*model, 0);
	wxString command = data->GetValue(*model, 1);

	bool ok = FbViewerDlg::Execute( this, type, command);
	if (ok) treeview->Replace(new TypeData(type, command));
	treeview->SetFocus();
}

void FbParamsDlg::OnDeleteType( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_TYPE_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	TypeData * data = wxDynamicCast(&item, TypeData);
	if (!data) return;

	wxString type = data->GetValue(*model, 0);
	wxString msg = _("Delete file type") + COLON + type;
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (!ok) return;

	m_del_type.Add(type);
	treeview->Delete();

	EnableTool(ID_TYPE_LIST, model->GetRowCount());
	treeview->SetFocus();
}

void FbParamsDlg::OnScriptActivated( wxTreeEvent & event )
{
	wxCommandEvent cmdEvent;
	OnModifyScript(cmdEvent);
}

void FbParamsDlg::OnTypeActivated( wxTreeEvent & event )
{
	wxCommandEvent cmdEvent;
	OnModifyType(cmdEvent);
}

void FbParamsDlg::SetColour(wxWindowID id, wxColour colour)
{
	wxColourPickerCtrl * control = (wxColourPickerCtrl*) FindWindow(id);
	if (control) control->SetColour(colour);
}

void FbParamsDlg::SetFont(wxWindowID id, wxFont font)
{
	wxFontPickerCtrl * control = (wxFontPickerCtrl*) FindWindow(id);
	if (control) control->SetSelectedFont(font);
}

void FbParamsDlg::OnFontClear( wxCommandEvent& event )
{
	wxFont font = wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT);
	SetFont(ID_FONT_MAIN, font);
	SetFont(ID_FONT_TOOL, font);
	SetFont(ID_FONT_HTML, font);
	SetFont(ID_FONT_DLG, font);

	wxColour colour = * wxBLACK;
	SetColour(ID_COLOUR_MAIN, colour);
	SetColour(ID_COLOUR_TOOL, colour);
	SetColour(ID_COLOUR_HTML, colour);
	SetColour(ID_COLOUR_DLG, colour);
}

void FbParamsDlg::OnLetterClicked(wxCommandEvent& event)
{
/*
	int pos = event.GetId() - ID_LETTERS;
	if (0 <= pos && pos < (int)m_letters.Length()) {
		wxKeyEvent event(wxEVT_CHAR);
		event.m_keyCode = wxT('%');
		m_text.EmulateKeyPress(event);
		event.m_keyCode = m_letters[pos];
		m_text.EmulateKeyPress(event);
	}
*/
}

void FbParamsDlg::EnableTool(wxWindowID id, bool enable)
{
	wxToolBar * toolbar = wxDynamicCast(FindWindow(++id), wxToolBar);
	if (toolbar) {
		toolbar->EnableTool(++id, true);
		toolbar->EnableTool(++id, enable);
		toolbar->EnableTool(++id, enable);
	}
}

void FbParamsDlg::OnModel( FbModelEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(event.GetId()), FbTreeViewCtrl);
	if (treeview) {
		FbModel * model = event.GetModel();
		EnableTool(event.GetId(), model->GetRowCount());
		treeview->AssignModel(model);
		if (event.GetId() == ID_SCRIPT_LIST) FillFormats(treeview, model);
	} else {
		delete event.GetModel();
	}
}

void FbParamsDlg::FillFormats(FbTreeViewCtrl * treeview, FbModel * model)
{
	FbChoiceInt * typelist = wxDynamicCast(FindWindow(ID_FILE_FORMAT), FbChoiceInt);
	if (!typelist) return;

	int format = FbParams(FB_FILE_FORMAT);
	size_t count = model->GetRowCount();
	for (size_t i=1; i<=count; i++) {
		FbModelItem item = model->GetData(i);
		ScriptData * data = wxDynamicCast(&item, ScriptData);
		if (!data) continue;
		wxString name = _("filename"); name << wxT('.') << data->GetValue(*model);
		int code = data->GetCode();
		int index = typelist->Append(name, code);
		if (format == code) typelist->SetSelection(index);
	}
}

void FbParamsDlg::SaveData()
{
	FbLocalDatabase database;
	DeleteTypes(database);
	DeleteScripts(database);
	SaveScripts(database);
	SaveTypes(database);
}

void FbParamsDlg::DeleteTypes(wxSQLite3Database &database)
{
	size_t count = m_del_type.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM types WHERE file_type=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_del_type[i]);
		stmt.ExecuteUpdate();
	}
}

void FbParamsDlg::DeleteScripts(wxSQLite3Database &database)
{
	size_t count = m_del_scr.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM script WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_del_scr[i]);
		stmt.ExecuteUpdate();
	}
}

void FbParamsDlg::SaveScripts(wxSQLite3Database &database)
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_SCRIPT_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	wxString sql = wxT("INSERT OR REPLACE INTO script(id, name, text) values(?,?,?)");

	size_t count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		ScriptData * data = wxDynamicCast(&item, ScriptData);
		if (data && data->IsModified()) {
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, data->GetCode());
			stmt.Bind(2, data->GetValue(*model, 0));
			stmt.Bind(3, data->GetValue(*model, 1));
			stmt.ExecuteUpdate();
		}
	}
}

void FbParamsDlg::SaveTypes(wxSQLite3Database &database)
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindow(ID_TYPE_LIST), FbTreeViewCtrl);
	if (!treeview) return;

	FbListStore * model = wxDynamicCast(treeview->GetModel(), FbListStore);
	if (!model) return;

	wxString sql = wxT("INSERT OR REPLACE INTO types(file_type, command) values(?,?)");

	size_t count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		TypeData * data = wxDynamicCast(&item, TypeData);
		if (data && data->IsModified()) {
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, data->GetValue(*model, 0));
			stmt.Bind(2, data->GetValue(*model, 1));
			stmt.ExecuteUpdate();
		}
	}
}
