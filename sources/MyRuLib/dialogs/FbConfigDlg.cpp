#include <wx/wx.h>
#include <wx/tokenzr.h>
#include "FbConfigDlg.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbViewerDlg.h"
#include "FbCollection.h"
#include "FbDataPath.h"
#include "controls/FbCustomCombo.h"
#include "controls/FbChoiceCtrl.h"
#include "FbLogoBitmap.h"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbDirectoryDlg
//-----------------------------------------------------------------------------

FbDirectoryDlg::FbDirectoryDlg( wxWindow * parent, const wxString& title ) 
	: FbDialog( parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
	wxBoxSizer * sizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer * sizerTitle = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Directory name"));
	info->Wrap( -1 );
	sizerTitle->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxTextCtrl * text = new wxTextCtrl( this, ID_TITLE);
	sizerTitle->Add( text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	sizerMain->Add( sizerTitle, 0, wxEXPAND, 5 );
	
	wxBoxSizer * sizerTable = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer * boxDir = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Directory contents") ), wxVERTICAL );
	
	wxFlexGridSizer * sizerDir = new wxFlexGridSizer( 2 );
	sizerDir->AddGrowableCol( 1 );
	sizerDir->SetFlexibleDirection( wxBOTH );
	sizerDir->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FbChoiceStr * choiseDirType = new FbChoiceStr(this, ID_DIR_TYPE);
	choiseDirType->Append(wxT("Autoincrement"), wxT("AUTOINCREMENT"));
	choiseDirType->Append(wxT("Integer"), wxT("INTEGER"));
	choiseDirType->Append(wxT("Text"), wxT("TEXT"));
	choiseDirType->SetSelection(0);
	
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_FILE), wxT("File name") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_DATA), wxT("Table name") );
	Append( sizerDir, choiseDirType                     , wxT("Key type") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_CODE), wxT("Field: code") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_NAME), wxT("Field: name") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_INFO), wxT("Field: info") );
	Append( sizerDir, new wxTextCtrl( this, ID_DIR_PRNT), wxT("Field: parent") );
	
	boxDir->Add( sizerDir, 1, wxEXPAND, 5 );
	
	sizerTable->Add( boxDir, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer * boxRef = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Detailed records") ), wxVERTICAL );
	
	wxFlexGridSizer * sizerRef = new wxFlexGridSizer( 2 );
	sizerRef->AddGrowableCol( 1 );
	sizerRef->SetFlexibleDirection( wxBOTH );
	sizerRef->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	FbChoiceStr * choiseRefType = new FbChoiceStr(this, ID_REF_TYPE);
	choiseRefType->Append(wxT("Book ID"), wxT("ID"));
	choiseRefType->Append(wxT("MD5 sum"), wxT("MD5SUM"));
	choiseRefType->SetSelection(0);
	
	Append( sizerRef, new wxTextCtrl( this, ID_REF_FILE), wxT("File name") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_DATA), wxT("Table name") );
	Append( sizerRef, choiseRefType                     , wxT("Key type") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_CODE), wxT("Field: code") );
	Append( sizerRef, new wxTextCtrl( this, ID_REF_BOOK), wxT("Field: book") );
	
	boxRef->Add( sizerRef, 1, wxEXPAND, 5 );
	
	sizerTable->Add( boxRef, 1, wxEXPAND|wxALL, 5 );
	
	sizerMain->Add( sizerTable, 1, wxEXPAND, 5 );
	
	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( sizerMain );
	this->Layout();
	sizerMain->Fit( this );
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
		wxString value = item[i];
		wxWindow * window = FindWindow(ID_TITLE + i);
		if (window == NULL) {
			continue;
		} else if (wxTextCtrl * control = wxDynamicCast(window, wxTextCtrl)) {
			control->SetValue(value);
		} else if (FbChoiceStr * control = wxDynamicCast(window, FbChoiceStr)) {
			control->SetValue(value);
		}
	}
}

void FbDirectoryDlg::Get(FbModelItem & item) 
{
	size_t count = ID_LAST - ID_TITLE;
	for (size_t i = 0; i < count; i++) {
		wxString value;
		wxWindow * window = FindWindow(ID_TITLE + i);
		if (window == NULL) {
			continue;
		} else if (wxTextCtrl * control = wxDynamicCast(window, wxTextCtrl)) {
			value = control->GetValue();
		} else if (FbChoiceStr * control = wxDynamicCast(window, FbChoiceStr)) {
			value = control->GetValue();
		}
		item.SetValue(i, value);
	}
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

void FbConfigDlg::TypeThread::LoadTypes(wxSQLite3Database &database)
{
	wxString sql = wxT("SELECT b.file_type, t.command, CASE WHEN b.file_type='fb2' THEN 1 ELSE 2 END AS key FROM (SELECT DISTINCT LOWER(file_type) AS file_type FROM books GROUP BY file_type UNION SELECT DISTINCT file_type FROM types UNION SELECT 'fb2' UNION SELECT 'pdf' UNION SELECT 'djvu' UNION SELECT 'txt') AS b LEFT JOIN types as t ON b.file_type = t.file_type ORDER BY key, b.file_type");

	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
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

void FbConfigDlg::RefsThread::LoadTables(wxSQLite3Database &database)
{
	if (!database.TableExists(wxT("tables"))) {
		FbModelEvent(ID_TYPE_LIST, new FbListStore).Post(m_frame);
		return;
	}
	
	wxString sql = wxString::Format(wxT("SELECT id, %s from tables ORDER BY 2"), GetFields().c_str());

	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
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
	m_treeview.AddColumn(0, _("Extension"), 50);
	m_treeview.AddColumn(1, _("Program"), 300);
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

void FbConfigDlg::PanelType::Save(wxSQLite3Database &database)
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	size_t count = m_deleted.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM types WHERE file_type=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_deleted[i]);
		stmt.ExecuteUpdate();
	}

	count = model->GetRowCount();
	for (size_t i = 1; i <= count; i++) {
		FbModelItem item = model->GetData(i);
		TypeData * data = wxDynamicCast(&item, TypeData);
		if (data && data->IsModified()) {
			wxString sql = wxT("INSERT OR REPLACE INTO types(file_type, command) values(?,?)");
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
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
	
	wxString title = wxGetTextFromUser(_("Create new directory"), _("Settings"));
	title = title.Trim(false).Trim(true).Lower();
	if (title.IsEmpty()) return;
	
	size_t num = 0;
	while (true) {
		num++;
		if (m_database.TableExists(wxString(wxT("dir")) << num)) continue;
		if (m_database.TableExists(wxString(wxT("ref")) << num)) continue;
		break;
	}
	
	wxString sql;
	
	sql = wxT("CREATE TABLE dir%d(code INTEGER PRIMARY KEY,name VARCHAR(128),info TEXT,parent INTEGER NOT NULL)");
	sql = wxString::Format(sql, num);
	m_database.ExecuteUpdate(sql);
	
	sql = wxT("CREATE INDEX dir%d_parent ON dir%d(parent)");
	sql = wxString::Format(sql, num, num);
	m_database.ExecuteUpdate(sql);

	sql = wxT("CREATE TABLE ref%d(code INTEGER, book INTEGER, PRIMARY KEY(code, book))");
	sql = wxString::Format(sql, num);
	m_database.ExecuteUpdate(sql);
	
	sql = wxT("CREATE INDEX ref%d_book ON ref%d(book)");
	sql = wxString::Format(sql, num, num);
	m_database.ExecuteUpdate(sql);
	
	wxArrayString values;
	values.Add( title );
	values.Add( wxEmptyString );
	values.Add( wxString(wxT("dir")) << num );
	values.Add( wxT("AUTOINCREMENT") );
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

	FbDirectoryDlg dlg(NULL, _("Append directory"));
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
	
	FbDirectoryDlg dlg(NULL, wxT("Modify directory"));
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

	wxString msg = _("Delete directory") + COLON + item[0];
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

void FbConfigDlg::PanelRefs::Save(wxSQLite3Database &database)
{
	FbListStore * model = wxDynamicCast(m_treeview.GetModel(), FbListStore);
	if (!model) return;

	if (!m_database.TableExists(wxT("tables"))) {
		wxString sql  = wxT("CREATE TABLE tables(id INTEGER PRIMARY KEY AUTOINCREMENT, %s)");
		sql = wxString::Format(sql, GetFields().c_str());
		database.ExecuteUpdate(sql);
	}

	size_t count = m_deleted.Count();
	for (size_t i = 0; i < count; i++) {
		wxString sql = wxT("DELETE FROM tables WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
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
			wxSQLite3Statement stmt = database.PrepareStatement(sql);
			data->Assign(stmt);
			stmt.ExecuteUpdate();
		}
	}
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

//-----------------------------------------------------------------------------
//  FbConfigDlg::RefsData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbConfigDlg::RefsData, FbModelData)

FbConfigDlg::RefsData::RefsData(wxSQLite3ResultSet &result)
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

int FbConfigDlg::RefsData::Assign(wxSQLite3Statement & stmt)
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
	wxFlexGridSizer* fgSizerMain;
	fgSizerMain = new wxFlexGridSizer( 2, 0, 0 );
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

	wxComboBox * comboAddr = new wxComboBox( this, ID_DOWNLOAD_HOST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	comboAddr->Append( wxT("http://%h/b/%i/download") );
	comboAddr->Append( wxT("http://%h/get?nametype=orig&md5=&m") );
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
		{DB_DOWNLOAD_ADDR, FbConfigDlg::ID_DOWNLOAD_ADDR},
		{DB_DOWNLOAD_USER, FbConfigDlg::ID_DOWNLOAD_USER},
		{DB_DOWNLOAD_PASS, FbConfigDlg::ID_DOWNLOAD_PASS},
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
