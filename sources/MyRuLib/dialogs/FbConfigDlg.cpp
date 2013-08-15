#include <wx/wx.h>
#include <wx/tokenzr.h>
#include "FbConfigDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbString.h"
#include "FbViewerDlg.h"
#include "FbCollection.h"
#include "FbDataPath.h"
#include "FbLogoBitmap.h"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbDirectoryDlg
//-----------------------------------------------------------------------------

FbDirectoryDlg::FbDirectoryDlg( wxWindow * parent, const wxString& title )
	: FbDialog( parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
	wxBoxSizer * sizerMain = new wxBoxSizer( wxVERTICAL );

	wxTextCtrl * name;
	wxBoxSizer * sizer = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Classifier name"));
	info->Wrap( -1 );
	sizer->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	name = new wxTextCtrl( this, ID_TITLE);
	sizer->Add( name, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	sizerMain->Add( sizer, 0, wxEXPAND, 5 );

	wxBoxSizer * sizerTable = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBoxSizer * boxDir = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Classifier contents") ), wxVERTICAL );

	wxFlexGridSizer * sizerDir = new wxFlexGridSizer( 2 );
	sizerDir->AddGrowableCol( 1 );
	sizerDir->SetFlexibleDirection( wxBOTH );
	sizerDir->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Append( sizerDir, new wxTextCtrl( this, ID_DIR_FILE), _("File name") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_DATA), _("Table name") );
	Append( sizerDir, CreateDirType ( this, ID_DIR_TYPE), _("Key type") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_CODE), _("Field: code") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_NAME), _("Field: name") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_INFO), _("Field: info") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_PRNT), _("Field: parent") );

	boxDir->Add( sizerDir, 1, wxEXPAND, 5 );

	sizerTable->Add( boxDir, 1, wxEXPAND|wxALL, 5 );

	wxStaticBoxSizer * boxRef = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Detailed records") ), wxVERTICAL );

	wxFlexGridSizer * sizerRef = new wxFlexGridSizer( 2 );
	sizerRef->AddGrowableCol( 1 );
	sizerRef->SetFlexibleDirection( wxBOTH );
	sizerRef->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	Append( sizerRef, new wxTextCtrl( this, ID_REF_FILE), _("File name") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_DATA), _("Table name") );
	Append( sizerRef, CreateRefType ( this, ID_REF_TYPE), _("Key type") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_CODE), _("Field: code") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_BOOK), _("Field: book") );

	boxRef->Add( sizerRef, 1, wxEXPAND, 5 );

	sizerTable->Add( boxRef, 1, wxEXPAND|wxALL, 5 );

	sizerMain->Add( sizerTable, 1, wxEXPAND, 5 );

	sizer = new wxBoxSizer( wxHORIZONTAL );

	info = new wxStaticText( this, wxID_ANY, wxT("<custom-info>"));
	info->Wrap( -1 );
	sizer->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl * text = new wxTextCtrl( this, ID_FB2_CODE);
	sizer->Add( text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	sizerMain->Add( sizer, 0, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( sizerMain );
	this->Layout();
	sizerMain->Fit( this );

	name->SetFocus();

	this->Centre( wxBOTH );
}

FbChoiceStr * FbDirectoryDlg::CreateDirType(wxWindow *parent, wxWindowID winid)
{
	FbChoiceStr * choise = new FbChoiceStr(parent, winid);
	choise->Append(wxT("Autoincrement"), wxT("AUTOINCREMENT"));
	choise->Append(wxT("Integer"), wxT("INTEGER"));
	choise->Append(wxT("Text"), wxT("TEXT"));
	choise->SetSelection(0);
	return choise;
}

FbChoiceStr * FbDirectoryDlg::CreateRefType(wxWindow *parent, wxWindowID winid)
{
	FbChoiceStr * choise = new FbChoiceStr(parent, winid);
	choise->Append(wxT("Book ID"), wxT("ID"));
	choise->Append(wxT("MD5 sum"), wxT("MD5SUM"));
	choise->SetSelection(0);
	return choise;
}

void FbDirectoryDlg::Append( wxFlexGridSizer * sizer, wxControl * control, const wxString & title )
{
	wxStaticText * caption = new wxStaticText( this, wxID_ANY, title );
	caption->Wrap( -1 );
	sizer->Add( caption, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	sizer->Add( control, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
}

void FbDirectoryDlg::Set(const FbModelItem & item)
{
	size_t count = ID_LAST - ID_TITLE;
	for (size_t i = 0; i < count; i++) {
		wxWindowID winid = ID_TITLE + i;
		wxWindow * window = FindWindow(winid);
		if (window == NULL) {
			wxLogError(wxT("Control not found: %d"), winid);
			continue;
		} else if (wxTextCtrl * control = wxDynamicCast(window, wxTextCtrl)) {
			control->SetValue(item[i]);
		} else if (FbChoiceStr * control = wxDynamicCast(window, FbChoiceStr)) {
			control->SetValue(item[i]);
		} else {
			wxLogError(wxT("Control not found: %d"), winid);
		}
	}
}

void FbDirectoryDlg::Get(FbModelItem & item)
{
	size_t count = ID_LAST - ID_TITLE;
	for (size_t i = 0; i < count; i++) {
		wxWindowID winid = ID_TITLE + i;
		wxWindow * window = FindWindow(winid);
		if (window == NULL) {
			wxLogError(wxT("Control not found: %d"), winid);
			continue;
		} else if (wxTextCtrl * control = wxDynamicCast(window, wxTextCtrl)) {
			item.SetValue(i, control->GetValue());
		} else if (FbChoiceStr * control = wxDynamicCast(window, FbChoiceStr)) {
			item.SetValue(i, control->GetValue());
		} else {
			wxLogError(wxT("Control not found: %d"), winid);
		}
	}
}

//-----------------------------------------------------------------------------
//  FbDirectoryDlg::CreateDlg
//-----------------------------------------------------------------------------

bool FbDirectoryDlg::CreateDlg::Execute(wxWindow * parent, wxString & name, wxString & type)
{
	CreateDlg dlg(parent);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) {
		name = dlg.m_name->GetValue();
		type = dlg.m_type->GetValue();
	}
	return ok;
}

FbDirectoryDlg::CreateDlg::CreateDlg( wxWindow * parent )
	: FbDialog( parent, wxID_ANY, _("Create new classifier"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * sizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer * sizerText = new wxFlexGridSizer( 2 );
	sizerText->AddGrowableCol( 1 );
	sizerText->SetFlexibleDirection( wxBOTH );
	sizerText->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * info;

	info = new wxStaticText( this, wxID_ANY, _("Classifier name") );
	info->Wrap( -1 );
	sizerText->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_name = new wxTextCtrl( this, wxID_ANY );
	m_name->SetMinSize(wxSize(200, -1));
	sizerText->Add( m_name, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	info = new wxStaticText( this, wxID_ANY, _("Key type") );
	info->Wrap( -1 );
	sizerText->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_type = CreateDirType( this, wxID_ANY );
	sizerText->Add( m_type, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	sizerMain->Add( sizerText, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( sizerMain );
	this->Layout();
	sizerMain->Fit( this );

	m_name->SetFocus();

	this->Centre( wxBOTH );
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::PanelTool
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbConfigDlg::PanelTool, wxPanel)

BEGIN_EVENT_TABLE( FbConfigDlg::PanelTool, wxPanel )
	EVT_FB_MODEL(ID_TYPE_LIST, FbConfigDlg::PanelTool::OnModel)
END_EVENT_TABLE()

FbConfigDlg::PanelTool::PanelTool(wxWindow * parent)
	: wxPanel(parent)
{
}

void FbConfigDlg::PanelTool::EnableTool(bool enable)
{
	m_toolbar.EnableTool(ID_CREATE, true);
	m_toolbar.EnableTool(ID_APPEND, true);
	m_toolbar.EnableTool(ID_MODIFY, enable);
	m_toolbar.EnableTool(ID_DELETE, enable);
}

void FbConfigDlg::PanelTool::OnModel( FbModelEvent& event )
{
	FbModel * model = event.GetModel();
	EnableTool(model->GetRowCount());
	m_treeview.AssignModel(model);
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::TypeThread
//-----------------------------------------------------------------------------

void * FbConfigDlg::TypeThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);
	LoadTypes(database);
	return NULL;
}

void FbConfigDlg::TypeThread::LoadTypes(FbSQLite3Database &database)
{
	wxString sql = GetCommandSQL(wxT("main"));
	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (!result.IsOk()) return;
	FbListStore * model = new FbListStore;
	while ( result.NextRow() ) {
		wxString type = result.GetString(0);
		if (type.IsEmpty() || type == wxT("exe")) continue;
		model->Append(new TypeData(result));
	}
	FbModelEvent(ID_TYPE_LIST, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::RefsThread
//-----------------------------------------------------------------------------

void * FbConfigDlg::RefsThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);
	LoadTables(database);
	return NULL;
}

void FbConfigDlg::RefsThread::LoadTables(FbSQLite3Database &database)
{
	if (!database.TableExists(wxT("tables"))) {
		FbModelEvent(ID_TYPE_LIST, new FbListStore).Post(m_frame);
		return;
	}

	wxString sql = wxString::Format(wxT("SELECT id, %s from tables ORDER BY 2"), GetFields().c_str());

	FbSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (!result.IsOk()) return;
	FbListStore * model = new FbListStore;
	while ( result.NextRow() ) {
		model->Append(new RefsData(result));
	}
	FbModelEvent(ID_TYPE_LIST, model).Post(m_frame);
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::PanelType
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbConfigDlg::PanelType, FbConfigDlg::PanelTool )
	EVT_TOOL( ID_APPEND, FbConfigDlg::PanelType::OnAppend )
	EVT_TOOL( ID_MODIFY, FbConfigDlg::PanelType::OnModify )
	EVT_TOOL( ID_DELETE, FbConfigDlg::PanelType::OnDelete )
	EVT_TREE_ITEM_ACTIVATED(ID_TYPE_LIST, FbConfigDlg::PanelType::OnActivated)
END_EVENT_TABLE()

FbConfigDlg::PanelType::PanelType(wxWindow * parent)
	: PanelTool(parent), m_thread(this)
{
	wxBoxSizer * bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	m_toolbar.AddTool( ID_APPEND, _("Append"), wxBitmap(add_xpm))->Enable(false);
	m_toolbar.AddTool( ID_MODIFY, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	m_toolbar.AddTool( ID_DELETE, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	m_toolbar.Realize();
	bSizer->Add( &m_toolbar, 0, wxALL|wxEXPAND, 5 );

	m_treeview.Create( this, ID_TYPE_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_treeview.AddColumn(0, _("Extension"), 4);
	m_treeview.AddColumn(2, _("Program"), -10);
	bSizer->Add( &m_treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	SetSizer( bSizer );
	Layout();
	bSizer->Fit( this );

	m_thread.Execute();
}

FbConfigDlg::PanelType::~PanelType()
{
	m_thread.Close();
	m_thread.Wait();
}

void FbConfigDlg::PanelType::OnAppend( wxCommandEvent& event )
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
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
	m_treeview.Append(new TypeData(filetype));
	m_treeview.SetFocus();
	EnableTool(true);
}

void FbConfigDlg::PanelType::OnModify( wxCommandEvent& event )
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	TypeData * data = wxDynamicCast(&item, TypeData);
	if (!data) return;

	wxString title = _("Select the application to view files");
	wxString type = data->GetValue(*model, 0);
	wxString command = data->GetValue(*model, 1);

	bool ok = FbViewerDlg::Execute( this, type, command, true);
	if (ok) m_treeview.Replace(new TypeData(type, command));
	m_treeview.SetFocus();
}

void FbConfigDlg::PanelType::OnDelete( wxCommandEvent& event )
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	FbModelItem item = model->GetCurrent();
	TypeData * data = wxDynamicCast(&item, TypeData);
	if (!data) return;

	wxString type = data->GetValue(*model, 0);
	wxString msg = _("Delete file type") + COLON + type;
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (!ok) return;

	m_deleted.Add(type);

	m_treeview.Delete();
	m_treeview.SetFocus();
	EnableTool(m_treeview.GetCurrent());
}

void FbConfigDlg::PanelType::OnActivated( wxTreeEvent & event )
{
	wxCommandEvent cmdEvent;
	OnModify(cmdEvent);
}

void FbConfigDlg::PanelType::Save(FbSQLite3Database &database)
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	size_t count = m_deleted.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM types WHERE file_type=?");
		FbSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_deleted[i]);
		stmt.ExecuteUpdate();
	}

	count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		TypeData * data = wxDynamicCast(&item, TypeData);
		if (data && data->IsModified()) {
			wxString sql = wxT("INSERT OR REPLACE INTO types(file_type, command) values(?,?)");
			FbSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, data->GetValue(*model, 0));
			stmt.Bind(2, data->GetValue(*model, 1));
			stmt.ExecuteUpdate();
		}
	}
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::PanelRefs
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbConfigDlg::PanelRefs, FbConfigDlg::PanelTool )
	EVT_TOOL( ID_CREATE, FbConfigDlg::PanelRefs::OnCreate )
	EVT_TOOL( ID_APPEND, FbConfigDlg::PanelRefs::OnAppend )
	EVT_TOOL( ID_MODIFY, FbConfigDlg::PanelRefs::OnModify )
	EVT_TOOL( ID_DELETE, FbConfigDlg::PanelRefs::OnDelete )
	EVT_TREE_ITEM_ACTIVATED(ID_REFS_LIST, FbConfigDlg::PanelRefs::OnActivated)
END_EVENT_TABLE()

FbConfigDlg::PanelRefs::PanelRefs(wxWindow * parent, FbDatabase & database)
	: PanelTool(parent), m_thread(this), m_database(database)
{
	wxBoxSizer * bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	m_toolbar.AddTool( ID_CREATE, _("Create"), wxBitmap(add_xpm))->Enable(false);
	m_toolbar.AddTool( ID_APPEND, _("Append"), wxBitmap(add_xpm))->Enable(false);
	m_toolbar.AddTool( ID_MODIFY, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	m_toolbar.AddTool( ID_DELETE, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	m_toolbar.Realize();
	bSizer->Add( &m_toolbar, 0, wxALL|wxEXPAND, 5 );

	m_treeview.Create( this, ID_REFS_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES|fbTR_NO_HEADER);
	bSizer->Add( &m_treeview, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );

	SetSizer( bSizer );
	Layout();
	bSizer->Fit( this );

	m_thread.Execute();
}

FbConfigDlg::PanelRefs::~PanelRefs()
{
	m_thread.Close();
	m_thread.Wait();
}

void FbConfigDlg::PanelRefs::OnCreate( wxCommandEvent& event )
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	wxString name, type;
	if (!FbDirectoryDlg::CreateDlg::Execute(this, name, type)) return ;
	if (name.IsEmpty()) return;

	size_t num = 0;
	while (true) {
		num++;
		if (m_database.TableExists(wxString(wxT("dir")) << num)) continue;
		if (m_database.TableExists(wxString(wxT("ref")) << num)) continue;
		break;
	}

	wxString pkey = type;
	if (type != wxT("TEXT")) pkey = wxT("INTEGER");
	pkey << wxT(' ') << wxT("PRIMARY KEY");
	if (type == wxT("AUTOINCREMENT")) pkey << wxT(' ') << wxT("AUTOINCREMENT");

	wxString sql;

	sql = wxT("CREATE TABLE dir%d(code %s,name VARCHAR(128),info TEXT,parent INTEGER NOT NULL)");
	sql = wxString::Format(sql, num, pkey.c_str());
	m_database.ExecuteUpdate(sql);

	sql = wxT("CREATE INDEX dir%d_parent ON dir%d(parent)");
	sql = wxString::Format(sql, num, num);
	m_database.ExecuteUpdate(sql);

	wxString code = type;
	if (type != wxT("TEXT")) code = wxT("INTEGER");

	sql = wxT("CREATE TABLE ref%d(code %s, book INTEGER, PRIMARY KEY(code, book))");
	sql = wxString::Format(sql, num, code.c_str());
	m_database.ExecuteUpdate(sql);

	sql = wxT("CREATE INDEX ref%d_book ON ref%d(book)");
	sql = wxString::Format(sql, num, num);
	m_database.ExecuteUpdate(sql);

	wxArrayString values;
	values.Add( name );
	values.Add( wxEmptyString );
	values.Add( wxString(wxT("dir")) << num );
	values.Add( type );
	values.Add( wxT("code") );
	values.Add( wxT("name") );
	values.Add( wxT("info") );
	values.Add( wxT("parent") );
	values.Add( wxEmptyString );
	values.Add( wxString(wxT("ref")) << num );
	values.Add( wxT("ID") );
	values.Add( wxT("code") );
	values.Add( wxT("book") );
	values.Add( wxEmptyString );

	m_treeview.Append(new RefsData(values));
	m_treeview.SetFocus();
	EnableTool(true);
}

void FbConfigDlg::PanelRefs::OnAppend( wxCommandEvent& event )
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	FbDirectoryDlg dlg(NULL, _("Append classifier"));
	if (dlg.ShowModal() == wxID_OK) {
		RefsData * data = new RefsData(wxArrayString());
		FbModelItem item(*model, data);
		dlg.Get(item);
		m_treeview.Append(data);
	}
}

void FbConfigDlg::PanelRefs::OnModify( wxCommandEvent& event )
{
	FbModelItem item = m_treeview.GetCurrent();
	if (!item) return;

	FbDirectoryDlg dlg(NULL, wxT("Modify classifier"));
	dlg.Set(item);
	if (dlg.ShowModal() == wxID_OK) {
		dlg.Get(item);
		m_treeview.Refresh();
	}
}

void FbConfigDlg::PanelRefs::OnDelete( wxCommandEvent& event )
{
	FbModelItem item = m_treeview.GetCurrent();
	RefsData * data = wxDynamicCast(&item, RefsData);
	if (!data) return;

	wxString msg = _("Delete classifier") + COLON + item[0];
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (!ok) return;

	int code = data->GetCode();
	if (code) m_deleted.Add(code);

	m_treeview.Delete();
	m_treeview.SetFocus();
	EnableTool(m_treeview.GetCurrent());
}

void FbConfigDlg::PanelRefs::OnActivated( wxTreeEvent & event )
{
	wxCommandEvent cmdEvent;
	OnModify(cmdEvent);
}

void FbConfigDlg::PanelRefs::Save(FbSQLite3Database &database)
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	{
		wxString sql  = wxT("CREATE TABLE IF NOT EXISTS tables(id INTEGER PRIMARY KEY AUTOINCREMENT, %s)");
		sql = wxString::Format(sql, GetFields().c_str());
		database.ExecuteUpdate(sql);
	}

	size_t count = m_deleted.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM tables WHERE id=?");
		FbSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_deleted[i]);
		stmt.ExecuteUpdate();
	}

	wxString values;
	wxString params;
	{
		bool next = false;
		wxStringTokenizer tkz(GetFields(), wxT(','), wxTOKEN_STRTOK);
		while (tkz.HasMoreTokens()) {
			if (next) {
				values << wxT(',');
				params << wxT(',');
			}
			values << tkz.GetNextToken() << wxT("=?");
			params << wxT("?");
			next = true;
		}
	}

	wxString insert = wxString::Format(wxT("INSERT INTO tables(%s)VALUES(%s)"), GetFields().c_str(), params.c_str());
	wxString update = wxString::Format(wxT("UPDATE tables SET %s WHERE id="), values.c_str());

	count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		RefsData * data = wxDynamicCast(&item, RefsData);
		if (data && data->IsModified()) {
			int code = data->GetCode();
			wxString sql = code ? update : insert;
			if (code) sql << code;
			FbSQLite3Statement stmt = database.PrepareStatement(sql);
			data->Assign(stmt);
			stmt.ExecuteUpdate();
		}
	}
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::TypeData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbConfigDlg::TypeData, FbModelData)

FbConfigDlg::TypeData::TypeData(FbSQLite3ResultSet &result)
	: m_type(result.GetString(0)), m_command(result.GetString(1)), m_modified(false)
{
}

wxString FbConfigDlg::TypeData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0: return m_type;
		case 1: return m_command;
		case 2: return (m_command == wxT('*')) ? fbT("~CR3") : m_command;
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbConfigDlg::RefsData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbConfigDlg::RefsData, FbModelData)

FbConfigDlg::RefsData::RefsData(FbSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_modified(false)
{
	size_t count = result.GetColumnCount();
	for (size_t i = 1; i < count; i++) {
		m_values.Add(result.GetString(i));
	}
}

FbConfigDlg::RefsData::RefsData(const wxArrayString & values)
	: m_code(0), m_values(values), m_modified(true)
{
}

wxString FbConfigDlg::RefsData::GetValue(FbModel & model, size_t col) const
{
	return col < m_values.Count() ? m_values[col] : wxString();
}

int FbConfigDlg::RefsData::Assign(FbSQLite3Statement & stmt)
{
	size_t count = m_values.Count();
	for (size_t i = 0; i < count; i++) {
		stmt.Bind(i + 1, m_values[i]);
	}
	return count;
}

void FbConfigDlg::RefsData::SetValue(FbModel & model, size_t col, const wxString &value)
{
	while (col >= m_values.Count()) m_values.Add(wxEmptyString);
	m_values[col] = value;
}

//-----------------------------------------------------------------------------
//  FbConfigDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbConfigDlg, wxDialog )
	EVT_BUTTON( ID_LIBRARY_DIR, FbConfigDlg::OnSelectFolderClick )
END_EVENT_TABLE()

wxString FbConfigDlg::GetFields()
{
	return wxT("title,dir_file,dir_data,dir_type,dir_code,dir_name,dir_info,dir_prnt,ref_file,ref_data,ref_type,ref_code,ref_book,fb2_code");
}

FbConfigDlg::PanelMain::PanelMain(wxWindow *parent)
	:wxPanel(parent)
{
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticText * m_staticText1 = new wxStaticText( this, wxID_ANY, _("Library name:"));
	m_staticText1->Wrap( -1 );
	bSizerMain->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxTextCtrl * m_textCtrl1 = new wxTextCtrl( this, ID_LIBRARY_TITLE);
	m_textCtrl1->SetMinSize( wxSize( 300,-1 ) );

	bSizerMain->Add( m_textCtrl1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	wxStaticText * m_staticText2 = new wxStaticText( this, wxID_ANY, _("Library folder:"));
	m_staticText2->Wrap( -1 );
	bSizerMain->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	FbCustomCombo * m_comboDir = new FbCustomCombo( this, ID_LIBRARY_DIR);
	m_comboDir->SetMinSize( wxSize( 300,-1 ) );

	bSizerMain->Add( m_comboDir, 0, wxEXPAND|wxALL, 5 );

	wxStaticText * m_staticText4 = new wxStaticText( this, wxID_ANY, _("Short description:"));
	m_staticText4->Wrap( -1 );
	bSizerMain->Add( m_staticText4, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	wxTextCtrl * m_textCtrl4 = new wxTextCtrl( this, ID_LIBRARY_DESCR, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCtrl4->SetMinSize( wxSize( 200, 60 ) );

	bSizerMain->Add( m_textCtrl4, 1, wxALL|wxEXPAND, 5 );
	this->SetSizer( bSizerMain );
	this->Layout();
}

FbConfigDlg::PanelInet::PanelInet(wxWindow *parent)
	:wxPanel(parent)
{
	wxFlexGridSizer * fgSizerMain = new wxFlexGridSizer( 2 );
	fgSizerMain->AddGrowableCol( 1 );
	fgSizerMain->SetFlexibleDirection( wxBOTH );
	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * text1 = new wxStaticText( this, wxID_ANY, _("Host name:"));
	text1->Wrap( -1 );
	fgSizerMain->Add( text1, 0, wxALL, 5 );

	wxComboBox * comboHost = new wxComboBox( this, ID_DOWNLOAD_HOST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	comboHost->Append( wxT("flibusta.net") );
	comboHost->Append( wxT("lib.rus.ec") );
	comboHost->Append( wxT("lib.ololo.cc") );
	fgSizerMain->Add( comboHost, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text4 = new wxStaticText( this, wxID_ANY, _("Address:"));
	text4->Wrap( -1 );
	fgSizerMain->Add( text4, 0, wxALL, 5 );

	wxComboBox * comboAddr = new wxComboBox( this, ID_DOWNLOAD_ADDR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	comboAddr->Append( wxT("http://%h/b/%i/download") );
	comboAddr->Append( wxT("http://%h/get?nametype=orig&md5=%s") );
	fgSizerMain->Add( comboAddr, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text2 = new wxStaticText( this, wxID_ANY, _("User name:"));
	text2->Wrap( -1 );
	fgSizerMain->Add( text2, 0, wxALL, 5 );

	wxTextCtrl * editUser = new wxTextCtrl( this, ID_DOWNLOAD_USER);
	fgSizerMain->Add( editUser, 0, wxALL|wxEXPAND, 5 );

	wxStaticText * text3 = new wxStaticText( this, wxID_ANY, _("Password:"));
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

FbConfigDlg::FbConfigDlg( wxWindow * parent )
	: FbDialog( parent, wxID_ANY, _("Library options"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_notebook.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	m_notebook.AddPage( new PanelMain(&m_notebook), _("General"),    true );
	m_notebook.AddPage( new PanelInet(&m_notebook), _("Network"),    false );
	m_notebook.AddPage( new PanelType(&m_notebook), _("File types"), false );
	m_notebook.AddPage( new PanelRefs(&m_notebook, m_database), _("Tables"), false );
	bSizerMain->Add( &m_notebook, 1, wxEXPAND | wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();
	bSizerMain->Fit( this );

	this->Centre( wxBOTH );
}

void FbConfigDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxComboCtrl * control = wxDynamicCast(FindWindow(event.GetId()), wxComboCtrl);
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
		{DB_DOWNLOAD_USER, FbConfigDlg::ID_DOWNLOAD_USER},
		{DB_DOWNLOAD_PASS, FbConfigDlg::ID_DOWNLOAD_PASS},
		{DB_DOWNLOAD_ADDR, FbConfigDlg::ID_DOWNLOAD_ADDR},
	};

	const size_t idsCount = sizeof(ids) / sizeof(Struct);

	for (size_t i = 0; i < idsCount; i++) {
		FbDialog::Assign(ids[i].control, ids[i].param, write);
	}

	if (write) {
		size_t count = m_notebook.GetPageCount();
		for (size_t i = 0; i < count; i++) {
			PanelTool * panel = wxDynamicCast(m_notebook.GetPage(i), PanelTool);
			if (panel) panel->Save(m_database);
		}
	}
}

bool FbConfigDlg::Execute(wxWindow* parent)
{
	FbConfigDlg dlg(parent);
	dlg.Assign(false);
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) {
		dlg.Assign(true);
		wxGetApp().UpdateLibPath();
		FbCollection::EmptyInfo();
	}
	return ok;
}
