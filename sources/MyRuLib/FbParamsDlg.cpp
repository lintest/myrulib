#include <wx/statline.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
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
#include <wx/settings.h>
#include "FbParams.h"
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbParamsDlg.h"
#include "ZipReader.h"
#include "MyRuLibApp.h"
#include "FbViewerDlg.h"
#include "FbTreeView.h"
#include "FbDatabase.h"
#include "FbChoiceFormat.h"
#include "FbLocale.h"
#include "FbToolBar.h"
#include "FbComboBox.h"

//-----------------------------------------------------------------------------
//  FbParamsDlg::LoadThread
//-----------------------------------------------------------------------------

void * FbParamsDlg::LoadThread::Entry()
{
	FbCommonDatabase database;
	database.AttachConfig();
	LoadScripts(database);
	LoadTypes(database);
	return NULL;
}

void FbParamsDlg::LoadThread::LoadTypes(wxSQLite3Database &database)
{
	wxString sql = wxT("\
		SELECT \
			b.file_type, t.command, CASE WHEN b.file_type='fb2' THEN 1 ELSE 2 END AS key\
		FROM ( \
			 SELECT DISTINCT LOWER(file_type) AS file_type FROM books GROUP BY file_type \
			 UNION SELECT DISTINCT file_type FROM config.types \
			 UNION SELECT 'fb2' \
			 UNION SELECT 'pdf' \
			 UNION SELECT 'djvu' \
			 UNION SELECT 'txt' \
		) AS b LEFT JOIN config.types as t ON b.file_type = t.file_type \
		ORDER BY key, b.file_type \
	 ");
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
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

	wxToolBar * toolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER );
	FbToolBarImages images(*toolbar, wxT("%m"));
	{
		size_t length = m_letters.Length();
		for (size_t i = 0; i < length; i++) {
			int btnid = ID_LETTERS + i;
			wxString title = wxT('%'); title += m_letters[i];
			wxString help = helps[i];
			toolbar->AddTool(btnid, title, images[title], help);
			this->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ScriptDlg::OnLetterClicked));
		}
	}
	toolbar->Realize();

	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerName = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Extension"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	bSizerName->Add( info, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_name.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
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

	wxFlexGridSizer* fgSizerList;
	fgSizerList = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerList->AddGrowableCol( 1 );
	fgSizerList->SetFlexibleDirection( wxBOTH );
	fgSizerList->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	AppendItem(fgSizerList, _("List of authors and books:"), ID_FONT_MAIN);
	AppendItem(fgSizerList, _("Toolbar:"), ID_FONT_TOOL);
	AppendItem(fgSizerList, _("Information:"), ID_FONT_HTML);
	AppendItem(fgSizerList, _("Dialog windows:"), ID_FONT_DLG);

	bSizerMain->Add( fgSizerList, 0, wxEXPAND, 5 );

	wxButton * btnClear = new wxButton( this, ID_FONT_CLEAR, _("Reset fonts settings"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( btnClear, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	SetSizer( bSizerMain );
	bSizerMain->Fit( this );
	Layout();
	SetMinSize(GetBestSize());
}

void FbParamsDlg::PanelFont::AppendItem(wxFlexGridSizer* fgSizer, const wxString& name, wxWindowID winid)
{
	wxStaticText * stTitle;
	stTitle = new wxStaticText( this, wxID_ANY, name, wxDefaultPosition, wxDefaultSize, 0 );
	stTitle->Wrap( -1 );
	fgSizer->Add( stTitle, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxFontPickerCtrl * fpValue;
	fpValue = new wxFontPickerCtrl( this, winid, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE|wxFNTP_USE_TEXTCTRL );
	fpValue->SetMaxPointSize( 100 );
	fgSizer->Add( fpValue, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
}

//-----------------------------------------------------------------------------
//  FbParamsDlg::PanelInternet
//-----------------------------------------------------------------------------

FbParamsDlg::PanelInternet::PanelInternet(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxCheckBox * m_checkBox13 = new wxCheckBox( this, ID_AUTO_DOWNLD, _("Automatically begin files downloading"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_checkBox13, 0, wxEXPAND|wxALL, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	wxCheckBox * m_checkBox12 = new wxCheckBox( this, ID_USE_PROXY, _("Use proxy-server"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_checkBox12, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxComboBox * m_comboBox2 = new wxComboBox( this, ID_PROXY_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox2->Append( wxT("192.168.0.1:3128") );
	m_comboBox2->Append( wxT("172.16.0.1:3128") );
	m_comboBox2->Append( wxT("10.0.0.1:3128") );
	bSizer13->Add( m_comboBox2, 1, wxALL, 5 );

	bSizerMain->Add( bSizer13, 0, wxEXPAND, 5 );

	wxCheckBox * checkBox3 = new wxCheckBox( this, ID_HTTP_IMAGES, _("Load images for author's description"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkBox3, 0, wxALL, 5 );

	wxStaticText * m_staticText6 = new wxStaticText( this, wxID_ANY, _("Folder to save downloads:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizerMain->Add( m_staticText6, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxFileSelectorCombo * combo = new wxFileSelectorCombo( this, ID_DOWNLOAD_DIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	wxCheckBox * m_checkBox14 = new wxCheckBox( this, ID_DEL_DOWNLOAD, _("Delete downloaded files when download query removed"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_checkBox14, 0, wxALL, 5 );

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

	wxToolBar * toolbar = new wxToolBar( this, ID_TYPE_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS );
	toolbar->SetToolBitmapSize(wxSize(0,0));
	toolbar->AddTool( ID_APPEND_TYPE, _("Append"), wxNullBitmap)->Enable(false);
	toolbar->AddTool( ID_MODIFY_TYPE, _("Modify"), wxNullBitmap)->Enable(false);
	toolbar->AddTool( ID_DELETE_TYPE, _("Delete"), wxNullBitmap)->Enable(false);
	toolbar->Realize();
	bSizerMain->Add( toolbar, 0, wxALL|wxEXPAND, 5 );

	FbTreeViewCtrl * treeview = new FbTreeViewCtrl( this, ID_TYPE_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	treeview->AddColumn(0, _("Extension"), 50);
	treeview->AddColumn(1, _("Program"), 300);
	bSizerMain->Add( treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

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
	wxCheckBox * checkbox;
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerLocale = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * typeText = new wxStaticText( this, wxID_ANY, _("Language"), wxDefaultPosition, wxDefaultSize, 0 );
	typeText->Wrap( -1 );
	bSizerLocale->Add( typeText, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	wxString filename = _("filename");
	FbChoiceFormat * localeChoice = new FbChoiceFormat( this, ID_LANG_LOCALE);
	FbLocale::Fill(localeChoice, FbParams::GetInt(FB_LANG_LOCALE));
	bSizerLocale->Add( localeChoice, 1, wxALL, 5 );

	bSizerMain->Add(bSizerLocale, 0, wxEXPAND);

	checkbox = new wxCheckBox( this, ID_SAVE_FULLPATH, _("Save full path of the file when importing"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	wxStaticText * text = new wxStaticText( this, wxID_ANY, _("Temporary folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	text->Wrap( -1 );
	bSizerMain->Add( text, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5 );

	wxFileSelectorCombo * combo = new wxFileSelectorCombo( this, ID_TEMP_DIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	#ifdef __WXGTK__
	{
		wxBoxSizer* bSizerDir = new wxBoxSizer( wxHORIZONTAL );

		wxStaticText * text = new wxStaticText( this, wxID_ANY, _("Wine temp folder:"), wxDefaultPosition, wxDefaultSize, 0 );
		text->Wrap( -1 );
		bSizerDir->Add( text, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

		wxTextCtrl * edit = new wxTextCtrl( this, ID_WINE_DIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		edit->SetMinSize( wxSize( 200,-1 ) );
		bSizerDir->Add( edit, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		bSizerMain->Add( bSizerDir, 0, wxEXPAND, 5 );
	}
	#endif

	checkbox = new wxCheckBox( this, ID_TEMP_DEL, _("Delete temporary files when you exit the program"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_REMOVE_FILES, _("Delete files when you delete a book"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

	checkbox = new wxCheckBox( this, ID_CLEAR_LOG, _("Clear the log window when it closes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL, 5 );

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

	wxStaticText * m_staticText6 = new wxStaticText( this, wxID_ANY, _("Destination folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizerMain->Add( m_staticText6, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	wxFileSelectorCombo * combo = new wxFileSelectorCombo( this, ID_EXTERNAL_DIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	combo->SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( combo, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * format_info = new wxStaticText( this, wxID_ANY, _("Exported collection structure"), wxDefaultPosition, wxDefaultSize, 0 );
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

	wxToolBar * toolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER );
	FbToolBarImages images(*toolbar, wxT("%m"));
	{
		size_t length = letters.Length();
		for (size_t i = 0; i < length; i++) {
			int btnid = ID_LETTERS + i;
			wxString title = wxT('%'); title += letters[i];
			wxString help = helps[i];
			toolbar->AddTool(btnid, title, images[title], help);
			parent->Connect(btnid, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(FbParamsDlg::OnLetterClicked));
		}
	}
	toolbar->Realize();
	bSizerMain->Add(toolbar, 0, wxEXPAND|wxALL, 5);

	wxCheckBox * checkbox;

	checkbox = new wxCheckBox( this, ID_TRANSLIT_FOLDER, _("Transliterate folder name"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_TRANSLIT_FILE, _("Transliterate filename"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	checkbox = new wxCheckBox( this, ID_USE_UNDERSCORE, _("Replace spaces by underscores"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( checkbox, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * typeText = new wxStaticText( this, wxID_ANY, _("Format of exporting collection"), wxDefaultPosition, wxDefaultSize, 0 );
	typeText->Wrap( -1 );
	bSizerFormat->Add( typeText, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	wxString filename = _("filename");
	FbChoiceFormat * typeChoice = new FbChoiceFormat( this, ID_FILE_FORMAT);
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

	wxToolBar * toolbar = new wxToolBar( this, ID_SCRIPT_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS );
	toolbar->SetToolBitmapSize(wxSize(0,0));
	toolbar->AddTool( ID_APPEND_SCRIPT, _("Append"), wxNullBitmap)->Enable(false);
	toolbar->AddTool( ID_MODIFY_SCRIPT, _("Modify"), wxNullBitmap)->Enable(false);
	toolbar->AddTool( ID_DELETE_SCRIPT, _("Delete"), wxNullBitmap)->Enable(false);
	toolbar->Realize();
	bSizerMain->Add( toolbar, 0, wxALL|wxEXPAND, 5 );

	FbTreeViewCtrl * treeview = new FbTreeViewCtrl( this, ID_SCRIPT_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	treeview->AddColumn(0, _("Extension"), 10);
	treeview->AddColumn(1, _("Export script"), 40);
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

	#if defined(__WIN32__)
	long nbStyle = wxNB_MULTILINE;
	#else
	long nbStyle = wxNB_LEFT;
	#endif

	wxNotebook * notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, nbStyle );
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
	m_thread.Wait();
}

void FbParamsDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxComboCtrl * control = wxDynamicCast(FindWindowById(event.GetId()), wxComboCtrl);
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
		{FB_TEMP_DEL, ID_TEMP_DEL},
		{FB_TEMP_DIR, ID_TEMP_DIR},
		{FB_WINE_DIR, ID_WINE_DIR},
		{FB_DOWNLOAD_DIR, ID_DOWNLOAD_DIR},
		{FB_DEL_DOWNLOAD, ID_DEL_DOWNLOAD},
		{FB_EXTERNAL_DIR, ID_EXTERNAL_DIR},
		{FB_TRANSLIT_FOLDER, ID_TRANSLIT_FOLDER},
		{FB_TRANSLIT_FILE, ID_TRANSLIT_FILE},
		{FB_USE_UNDERSCORE, ID_USE_UNDERSCORE},
		{FB_FOLDER_FORMAT, ID_FOLDER_FORMAT},
		{FB_FONT_MAIN, ID_FONT_MAIN},
		{FB_FONT_HTML, ID_FONT_HTML},
		{FB_FONT_TOOL, ID_FONT_TOOL},
		{FB_FONT_DLG, ID_FONT_DLG},
		{FB_HTTP_IMAGES, ID_HTTP_IMAGES},
		{FB_REMOVE_FILES, ID_REMOVE_FILES},
		{FB_SAVE_FULLPATH, ID_SAVE_FULLPATH},
		{FB_CLEAR_LOG, ID_CLEAR_LOG},
		{FB_FILE_FORMAT, ID_FILE_FORMAT},
		{FB_LANG_LOCALE, ID_LANG_LOCALE},
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
		FbTempEraser::sm_erase = FbParams::GetInt(FB_TEMP_DEL);
		wxGetApp().Localize();
	}
};

void FbParamsDlg::OnAppendScript( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_SCRIPT_LIST), FbTreeViewCtrl);
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
	FbChoiceFormat * typelist = wxDynamicCast(FindWindowById(ID_FILE_FORMAT), FbChoiceFormat);
	if (typelist) typelist->Append(label, code);
}

void FbParamsDlg::OnModifyScript( wxCommandEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_SCRIPT_LIST), FbTreeViewCtrl);
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

	FbChoiceFormat * typelist = wxDynamicCast(FindWindowById(ID_FILE_FORMAT), FbChoiceFormat);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_SCRIPT_LIST), FbTreeViewCtrl);
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

	FbChoiceFormat * typelist = wxDynamicCast(FindWindowById(ID_FILE_FORMAT), FbChoiceFormat);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_TYPE_LIST), FbTreeViewCtrl);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_TYPE_LIST), FbTreeViewCtrl);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_TYPE_LIST), FbTreeViewCtrl);
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
void FbParamsDlg::SetFont(wxWindowID id, wxFont font)
{
	wxFontPickerCtrl * control = (wxFontPickerCtrl*) FindWindowById(id);
	if (control) control->SetSelectedFont(font);
}

void FbParamsDlg::OnFontClear( wxCommandEvent& event )
{
	wxFont font = wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT);
	SetFont(ID_FONT_MAIN, font);
	SetFont(ID_FONT_TOOL, font);
	SetFont(ID_FONT_HTML, font);
	SetFont(ID_FONT_DLG, font);
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
	wxToolBar * toolbar = wxDynamicCast(FindWindowById(++id), wxToolBar);
	if (toolbar) {
		toolbar->EnableTool(++id, true);
		toolbar->EnableTool(++id, enable);
		toolbar->EnableTool(++id, enable);
	}
}

void FbParamsDlg::OnModel( FbModelEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(event.GetId()), FbTreeViewCtrl);
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
	FbChoiceFormat * typelist = wxDynamicCast(FindWindowById(ID_FILE_FORMAT), FbChoiceFormat);
	if (!typelist) return;

	int format = FbParams::GetInt(FB_FILE_FORMAT);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_SCRIPT_LIST), FbTreeViewCtrl);
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
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(ID_TYPE_LIST), FbTreeViewCtrl);
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

