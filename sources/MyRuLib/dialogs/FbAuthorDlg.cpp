#include "FbAuthorDlg.h"
#include "models/FbAuthList.h"
#include "FbCollection.h"
#include "FbConst.h"
#include "FbLogoBitmap.h"
#include <wx/artprov.h>

//-----------------------------------------------------------------------------
//  FbAuthorSelectDlg
//-----------------------------------------------------------------------------

FbAuthorSelectDlg::FbAuthorSelectDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer * bSizerText = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Find"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	bSizerText->Add( info, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_text.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerText->Add( &m_text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerText, 0, wxEXPAND, 5 );

	m_alphabet.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( &m_alphabet, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_toolbar.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_HORZ_TEXT|wxTB_NODIVIDER );
	m_toolbar.AddTool( ID_APPEND, _("Append"), wxBitmap(add_xpm))->Enable(false);
	m_toolbar.AddTool( ID_MODIFY, _("Modify"), wxBitmap(mod_xpm))->Enable(false);
	m_toolbar.AddTool( ID_DELETE, _("Delete"), wxBitmap(del_xpm))->Enable(false);
	m_toolbar.Realize();

	bSizerMain->Add( &m_toolbar, 0, wxEXPAND|wxALL, 5 );

	m_treeview.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON );
	bSizerMain->Add( &m_treeview, 1, wxALL|wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();
	SetSize(GetBestSize());

	m_text.SetFocus();
}

FbAuthorSelectDlg::~FbAuthorSelectDlg()
{
}

//-----------------------------------------------------------------------------
//  FbAuthorModifyDlg
//-----------------------------------------------------------------------------

FbAuthorModifyDlg::FbAuthorModifyDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer * bSizerGrid = new wxFlexGridSizer( 2 );
	bSizerGrid->AddGrowableCol( 1 );
	bSizerGrid->SetFlexibleDirection( wxBOTH );
	bSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxTextCtrl * lastname = AppenName(bSizerGrid, ID_LAST_NAME, _("Surname"));
	AppenName(bSizerGrid, ID_FIRST_NAME,  _("Name"));
	AppenName(bSizerGrid, ID_MIDDLE_NAME, _("Middle-Name"));

	bSizerMain->Add( bSizerGrid, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();
	SetSize(GetBestSize());
	lastname->SetFocus();
}

wxTextCtrl * FbAuthorModifyDlg::AppenName(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption)
{
	wxStaticText * text = new wxStaticText( this, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, 0 );
	text->Wrap( -1 );
	parent->Add( text, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl * edit = new wxTextCtrl( this, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	edit->SetMinSize( wxSize( 300,-1 ) );
	parent->Add( edit, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	return edit;
}

int FbAuthorModifyDlg::Append(wxString &newname)
{
	FbAuthorModifyDlg dlg(_("Add author"));
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetFullName();
	return ok ? dlg.DoAppend() : 0;
}

int FbAuthorModifyDlg::Modify(int id, wxString &newname)
{
	FbAuthorModifyDlg dlg(_("Change author"), id);
	bool ok = dlg.Load(id) && dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetFullName();
	return ok ? dlg.DoUpdate() : 0;
}

wxString FbAuthorModifyDlg::GetFullName()
{
	AuthorItem author;
	GetValues(author);
	return author.GetFullName();
}

bool FbAuthorModifyDlg::Load(int id)
{
	AuthorItem author(id);
	bool ok = author.Load(m_database);
	SetValue(ID_LAST_NAME, author.last);
	SetValue(ID_FIRST_NAME, author.first);
	SetValue(ID_MIDDLE_NAME, author.middle);
	return ok;
}

wxString FbAuthorModifyDlg::GetValue(wxWindowID id)
{
	wxTextCtrl * control = (wxTextCtrl*) FindWindow(id);
	return control ? control->GetValue() : wxString(wxEmptyString);
}

void FbAuthorModifyDlg::SetValue(wxWindowID id, const wxString &text)
{
	wxTextCtrl * control = (wxTextCtrl*) FindWindow(id);
	if (control) control->SetValue(text);
}

int FbAuthorModifyDlg::FindAuthor()
{
	AuthorItem author(m_id);
	GetValues(author);
	return author.Find(m_database);
}

int FbAuthorModifyDlg::DoAppend()
{
	AuthorItem author;
	GetValues(author);
	return author.Save(m_database);
}

int FbAuthorModifyDlg::DoUpdate()
{
	if (m_exists) {
		ReplaceAuthor(m_id, m_exists);
		return m_exists;
	} else {
		DoModify();
		return m_id;
	}
}

void FbAuthorModifyDlg::GetValues(AuthorItem &author)
{
	author.last   = GetValue(ID_LAST_NAME).Trim(false).Trim(true);
	author.first  = GetValue(ID_FIRST_NAME).Trim(false).Trim(true);
	author.middle = GetValue(ID_MIDDLE_NAME).Trim(false).Trim(true);
}

void FbAuthorModifyDlg::DoModify()
{
	AuthorItem author(m_id);
	GetValues(author);
	author.Save(m_database);
	FbCollection::ResetAuth(m_id);
}

void FbAuthorModifyDlg::ReplaceAuthor(int old_id, int new_id)
{
	FbCommonDatabase m_database;
	wxSQLite3Transaction trans(&m_database);

	{
		wxString sql = wxT("UPDATE books SET id_author=?1 WHERE id_author=?2 AND NOT (id IN (SELECT id FROM books WHERE id_author=?1))");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, new_id);
		stmt.Bind(2, old_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = wxT("DELETE FROM books WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, old_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = wxT("DELETE FROM authors WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, old_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = wxT("DELETE FROM fts_auth WHERE docid=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, old_id);
		stmt.ExecuteUpdate();
	}

	trans.Commit();

	FbCollection::ResetAuth(old_id);
	FbCollection::ResetAuth(new_id);
}

void FbAuthorModifyDlg::EndModal(int retCode)
{
	if ( retCode == wxID_OK) {
		if (GetValue(ID_LAST_NAME).IsEmpty()) {
			wxMessageBox(_("\"Surname\" field is empty"), GetTitle());
			return;
		}
		m_exists = FindAuthor();
		if (m_exists) {
			wxString msg = _("Autor with same details aleready exists");
			wxString title = GetTitle() + wxT("...");
			if (m_id) {
				msg += wxT('\n');
				msg += _("Merge authors?");
				bool ok = wxMessageBox(msg, title, wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
				if (!ok) return;
			} else {
				wxMessageBox(msg, title, wxICON_EXCLAMATION);
				return;
			}
		}
	}
	FbDialog::EndModal(retCode);
}

//-----------------------------------------------------------------------------
//  FbAuthorReplaceDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbAuthorReplaceDlg, wxDialog )
	EVT_TEXT_ENTER( ID_FIND_TXT, FbAuthorReplaceDlg::OnFindEnter )
	EVT_BUTTON( ID_FIND_TXT, FbAuthorReplaceDlg::OnFindEnter )
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbAuthorReplaceDlg::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbAuthorReplaceDlg::OnArray)
	EVT_COMMAND(ID_MODEL_NUMBER, fbEVT_BOOK_ACTION, FbAuthorReplaceDlg::OnNumber)
END_EVENT_TABLE()

FbAuthorReplaceDlg::FbAuthorReplaceDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
		m_id(id), m_thread(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer * fgSizerGrid = new wxFlexGridSizer( 2 );
	fgSizerGrid->AddGrowableCol( 1 );
	fgSizerGrid->SetFlexibleDirection( wxBOTH );
	fgSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * m_static1 = new wxStaticText( this, wxID_ANY, _("Replace") + COLON );
	m_static1->Wrap( -1 );
	fgSizerGrid->Add( m_static1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_Text = new wxTextCtrl( this, ID_TEXT );
	m_Text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_Text->SetMinSize( wxSize( 300,-1 ) );

	fgSizerGrid->Add( m_Text, 0, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	wxStaticText * m_static2 = new wxStaticText( this, wxID_ANY, _("Find") + COLON );
	m_static2->Wrap( -1 );
	fgSizerGrid->Add( m_static2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_FindText = new FbSearchCombo( this, ID_FIND_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 250,-1 ) );
	fgSizerGrid->Add( m_FindText, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	bSizerMain->Add( fgSizerGrid, 0, wxEXPAND, 5 );

	m_MasterList = new FbTreeViewCtrl(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	bSizerMain->Add( m_MasterList, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	m_MasterList->AddColumn(0, _("Author"), -10, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 6, wxALIGN_RIGHT);

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	m_FindText->SetFocus();
}

FbAuthorReplaceDlg::~FbAuthorReplaceDlg()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Delete();
	}
	if (!m_MasterTemp.IsEmpty()) wxRemoveFile(m_MasterTemp);
}

bool FbAuthorReplaceDlg::Load()
{
	wxString sql = wxT("SELECT full_name, last_name FROM authors WHERE id=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, m_id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	if (result.NextRow()) {
		m_Text->SetValue(result.GetAsString(0));
		wxString last_name = result.GetAsString(1);
		if (!last_name.IsEmpty()) (m_thread = new FbAuthListThread(this, last_name, 0, m_MasterFile))->Execute();
		return true;
	}
	return false;
}

void FbAuthorReplaceDlg::OnFindEnter( wxCommandEvent& event )
{
	wxString text = m_FindText->GetValue();
	if (m_thread) {
		m_thread->Close();
		m_thread->Delete();
		m_thread = NULL;
	}
	if (!text.IsEmpty()) (m_thread = new FbAuthListThread(this, text, 0, m_MasterFile))->Execute();
}

int FbAuthorReplaceDlg::GetSelected()
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbAuthListData * data = wxDynamicCast(&item, FbAuthListData);
	return data ? data->GetCode() : 0;
}

void FbAuthorReplaceDlg::EndModal(int retCode)
{
	if ( retCode == wxID_OK) {
		int author = GetSelected();
		if (author) {
			wxString msg = _("Merge authors?");
			wxString title = GetTitle() + wxT("...");
			bool ok = wxMessageBox(msg, title, wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
			if (!ok) return;
		} else {
			wxMessageBox(_("Author not selected by replacement."), GetTitle());
			return;
		}
	}
	FbDialog::EndModal(retCode);
}

int FbAuthorReplaceDlg::DoUpdate()
{
	int selected = GetSelected();
	FbAuthorModifyDlg::ReplaceAuthor(m_id, selected);
	return selected;
}

wxString FbAuthorReplaceDlg::GetFullName()
{
	return m_MasterList->GetCurrent();
}

int FbAuthorReplaceDlg::Execute(int author, wxString& newname, const wxString& counter)
{
	FbAuthorReplaceDlg dlg(_("Replace author"), author);
	dlg.m_MasterFile = counter;
	bool ok = dlg.Load() && dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetFullName();
	return ok ? dlg.DoUpdate() : 0;
}

void FbAuthorReplaceDlg::OnModel( FbArrayEvent& event )
{
	wxArrayInt array = event.GetArray();
	int index = array.Index(m_id);
	if (index != wxNOT_FOUND) array.RemoveAt(index);

	FbAuthListModel * model = new FbAuthListModel(array);
	model->SetCounter(m_MasterFile);
	m_MasterList->AssignModel(model);
}

void FbAuthorReplaceDlg::OnArray( FbArrayEvent& event )
{
	wxArrayInt array = event.GetArray();
	int index = array.Index(m_id);
	if (index != wxNOT_FOUND) array.RemoveAt(index);

	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->Append(array);
	m_MasterList->Refresh();
}

bool FbAuthorReplaceDlg::Delete(FbModel &model)
{
	FbModelItem item = model.GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return false;

	int id = current->GetCode();
	int count = FbCommonDatabase().Int(id, wxT("SELECT COUNT(DISTINCT id) FROM books WHERE id_author=?"));

	wxString msg = _("Delete author") + COLON + current->GetValue(model);
	if (count) msg << (wxString)wxT("\n") <<  wxString::Format(_("and all of author's books (%d pcs.)?"), count);
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		wxString sql1 = wxString::Format(wxT("DELETE FROM books WHERE id_author=?"), id);
		wxString sql2 = wxString::Format(wxT("DELETE FROM authors WHERE id=%d"), id);
		FbCommonDatabase database;
		database.ExecuteUpdate(sql1);
		database.ExecuteUpdate(sql2);
	}
	return ok;
}

void FbAuthorReplaceDlg::OnNumber(wxCommandEvent& event)
{
	m_MasterTemp = m_MasterFile = event.GetString();
	FbAuthListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbAuthListModel);
	if (model) model->SetCounter(m_MasterFile);
	m_MasterList->Refresh();
}
