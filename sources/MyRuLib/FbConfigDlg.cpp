#include <wx/wx.h>
#include "FbConfigDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "controls/FbTreeView.h"
#include "FbViewerDlg.h"
#include "FbCollection.h"
#include "FbDataPath.h"
#include "controls/FbCustomCombo.h"
#include "FbLogoBitmap.h"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbConfigDlg::LoadThread
//-----------------------------------------------------------------------------

void * FbConfigDlg::LoadThread::Entry()
{
	FbCommonDatabase database;
	LoadTypes(database);
	return NULL;
}

void FbConfigDlg::LoadThread::LoadTypes(wxSQLite3Database &database)
{
	wxString sql = wxT("\
		SELECT \
			b.file_type, t.command, CASE WHEN b.file_type='fb2' THEN 1 ELSE 2 END AS key\
		FROM ( \
			 SELECT DISTINCT LOWER(file_type) AS file_type FROM books GROUP BY file_type \
			 UNION SELECT DISTINCT file_type FROM types \
			 UNION SELECT 'fb2' \
			 UNION SELECT 'pdf' \
			 UNION SELECT 'djvu' \
			 UNION SELECT 'txt' \
		) AS b LEFT JOIN types as t ON b.file_type = t.file_type \
		ORDER BY key, b.file_type \
	 ");

	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	FbListStore * model = new FbListStore;
	while ( result.NextRow() ) {
		wxString type = result.GetString(0);
		if (type.IsEmpty() || type == wxT("exe")) continue;
		model->Append(new TypeData(result));
	}
	FbModelEvent(ID_TYPE_LIST, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::PanelTypes
//-----------------------------------------------------------------------------

FbConfigDlg::PanelTypes::PanelTypes(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer * bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	wxToolBar * toolbar = new wxToolBar( this, ID_TYPE_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	toolbar->AddTool( ID_APPEND_TYPE, _("Append"), wxBitmap(add_xpm))->Enable(false);
	toolbar->AddTool( ID_MODIFY_TYPE, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	toolbar->AddTool( ID_DELETE_TYPE, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	toolbar->Realize();
	bSizer->Add( toolbar, 0, wxALL|wxEXPAND, 5 );

	FbTreeViewCtrl * treeview = new FbTreeViewCtrl( this, ID_TYPE_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	treeview->AddColumn(0, _("Extension"), 50);
	treeview->AddColumn(1, _("Program"), 300);
	bSizer->Add( treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	SetSizer( bSizer );
	Layout();
	bSizer->Fit( this );
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::TypeData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbConfigDlg::TypeData, FbModelData)

FbConfigDlg::TypeData::TypeData(wxSQLite3ResultSet &result)
	: m_type(result.GetString(0)), m_command(result.GetString(1)), m_modified(false)
{
}

wxString FbConfigDlg::TypeData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0: return m_type;
		case 1: return m_command;
		default: return wxEmptyString;
	}
}

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbConfigDlg, wxDialog )
	EVT_BUTTON( ID_LIBRARY_DIR, FbConfigDlg::OnSelectFolderClick )
	EVT_TOOL( ID_APPEND_TYPE, FbConfigDlg::OnAppendType )
	EVT_TOOL( ID_MODIFY_TYPE, FbConfigDlg::OnModifyType )
	EVT_TOOL( ID_DELETE_TYPE, FbConfigDlg::OnDeleteType )
	EVT_TREE_ITEM_ACTIVATED(ID_TYPE_LIST, FbConfigDlg::OnTypeActivated)
	EVT_FB_MODEL(ID_TYPE_LIST, FbConfigDlg::OnModel)
END_EVENT_TABLE()

FbConfigDlg::PanelMain::PanelMain(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticText * m_staticText1 = new wxStaticText( this, wxID_ANY, _("Library name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizerMain->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxTextCtrl * m_textCtrl1 = new wxTextCtrl( this, ID_LIBRARY_TITLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl1->SetMinSize( wxSize( 300,-1 ) );

	bSizerMain->Add( m_textCtrl1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	wxStaticText * m_staticText2 = new wxStaticText( this, wxID_ANY, _("Library folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizerMain->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	FbCustomCombo * m_comboDir = new FbCustomCombo( this, ID_LIBRARY_DIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_comboDir->SetMinSize( wxSize( 300,-1 ) );

	bSizerMain->Add( m_comboDir, 0, wxEXPAND|wxALL, 5 );

	wxStaticText * m_staticText4 = new wxStaticText( this, wxID_ANY, _("Short description:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizerMain->Add( m_staticText4, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxTextCtrl * m_textCtrl4 = new wxTextCtrl( this, ID_LIBRARY_DESCR, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCtrl4->SetMinSize( wxSize( 200, 60 ) );

	bSizerMain->Add( m_textCtrl4, 1, wxALL|wxEXPAND, 5 );
	this->SetSizer( bSizerMain );
	this->Layout();
}

FbConfigDlg::PanelInternet::PanelInternet(wxWindow *parent)
	:wxPanel(parent)
{
	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 0, 0 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * text1 = new wxStaticText( this, wxID_ANY, _("Host name:"), wxDefaultPosition, wxDefaultSize, 0 );
	text1->Wrap( -1 );
	fgSizerMain->Add( text1, 0, wxALL, 5 );

	wxComboBox * comboHost = new wxComboBox( this, ID_DOWNLOAD_HOST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	comboHost->Append( wxT("flibusta.net") );
	comboHost->Append( wxT("lib.rus.ec") );
	comboHost->Append( wxT("lib.ololo.cc") );
	fgSizerMain->Add( comboHost, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text4 = new wxStaticText( this, wxID_ANY, _("Address:"), wxDefaultPosition, wxDefaultSize, 0 );
	text4->Wrap( -1 );
	fgSizerMain->Add( text4, 0, wxALL, 5 );

	wxComboBox * comboAddr = new wxComboBox( this, ID_DOWNLOAD_HOST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	comboAddr->Append( wxT("http://%h/b/%i/download") );
	comboAddr->Append( wxT("http://%h/get?nametype=orig&md5=&m") );
	fgSizerMain->Add( comboAddr, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text2 = new wxStaticText( this, wxID_ANY, _("User name:"), wxDefaultPosition, wxDefaultSize, 0 );
	text2->Wrap( -1 );
	fgSizerMain->Add( text2, 0, wxALL, 5 );

	wxTextCtrl * editUser = new wxTextCtrl( this, ID_DOWNLOAD_USER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerMain->Add( editUser, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text3 = new wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	text3->Wrap( -1 );
	fgSizerMain->Add( text3, 0, wxALL, 5 );

	wxTextCtrl * editPass = new wxTextCtrl( this, ID_DOWNLOAD_PASS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	fgSizerMain->Add( editPass, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizerMain );
	this->Layout();
}

//-----------------------------------------------------------------------------
//  FbConfigDlg
//-----------------------------------------------------------------------------

FbConfigDlg::FbConfigDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: FbDialog( parent, id, title, pos, size, style ), m_thread(this)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxNotebook * notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	notebook->AddPage( new PanelMain(notebook), _("General"), true );
	notebook->AddPage( new PanelInternet(notebook), _("Network"), false );
	notebook->AddPage( new PanelTypes(notebook), _("File types"), false );
	bSizerMain->Add( notebook, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	if (m_thread.Create() == wxTHREAD_NO_ERROR) m_thread.Run();
}

FbConfigDlg::~FbConfigDlg()
{
	m_thread.Wait();
}

void FbConfigDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxComboCtrl * control = wxDynamicCast(FindWindowById(event.GetId()), wxComboCtrl);
	if (!control) return;

	wxDirDialog dlg(
		this,
		_("Select folder"),
		control->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK) {
		wxString filepath = FbStandardPaths::MakeRelative(dlg.GetPath(), wxGetApp().GetLibFile());
		control->SetValue( filepath );
	}
}

void FbConfigDlg::Assign(bool write)
{
	struct Struct {
		int param;
		ID control;
	};

	const Struct ids[] = {
		{DB_LIBRARY_TITLE, FbConfigDlg::ID_LIBRARY_TITLE},
		{DB_LIBRARY_DIR,   FbConfigDlg::ID_LIBRARY_DIR},
		{DB_LIBRARY_DESCR, FbConfigDlg::ID_LIBRARY_DESCR},
		{DB_DOWNLOAD_HOST, FbConfigDlg::ID_DOWNLOAD_HOST},
		{DB_DOWNLOAD_ADDR, FbConfigDlg::ID_DOWNLOAD_ADDR},
		{DB_DOWNLOAD_USER, FbConfigDlg::ID_DOWNLOAD_USER},
		{DB_DOWNLOAD_PASS, FbConfigDlg::ID_DOWNLOAD_PASS},
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	for (size_t i=0; i<idsCount; i++) {
		FbDialog::Assign(ids[i].control, ids[i].param, write);
	}

	if (write) SaveTypes(m_database);
}

void FbConfigDlg::Execute(wxWindow* parent)
{
	FbConfigDlg dlg(parent, wxID_ANY, _("Library options"), wxDefaultPosition, wxDefaultSize);
	dlg.Assign(false);
	if (dlg.ShowModal() == wxID_OK) {
		dlg.Assign(true);
		wxGetApp().UpdateLibPath();
		FbCollection::EmptyInfo();
	}
}

void FbConfigDlg::SaveTypes(wxSQLite3Database &database)
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

void FbConfigDlg::OnAppendType( wxCommandEvent& event )
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

void FbConfigDlg::OnModifyType( wxCommandEvent& event )
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

	bool ok = FbViewerDlg::Execute( this, type, command, true);
	if (ok) treeview->Replace(new TypeData(type, command));
	treeview->SetFocus();
}

void FbConfigDlg::OnDeleteType( wxCommandEvent& event )
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

void FbConfigDlg::OnTypeActivated( wxTreeEvent & event )
{
	wxCommandEvent cmdEvent;
	OnModifyType(cmdEvent);
}

void FbConfigDlg::OnModel( FbModelEvent& event )
{
	FbTreeViewCtrl * treeview = wxDynamicCast(FindWindowById(event.GetId()), FbTreeViewCtrl);
	if (treeview) {
		FbModel * model = event.GetModel();
		EnableTool(event.GetId(), model->GetRowCount());
		treeview->AssignModel(model);
	} else {
		delete event.GetModel();
	}
}

void FbConfigDlg::EnableTool(wxWindowID id, bool enable)
{
	wxToolBar * toolbar = wxDynamicCast(FindWindowById(++id), wxToolBar);
	if (toolbar) {
		toolbar->EnableTool(++id, true);
		toolbar->EnableTool(++id, enable);
		toolbar->EnableTool(++id, enable);
	}
}

