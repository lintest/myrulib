#include "FbAuthorDlg.h"
#include "FbAuthList.h"
#include "FbCollection.h"
#include "FbConst.h"
#include <wx/artprov.h>

FbAuthorModifyDlg::FbAuthorModifyDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* bSizerGrid;
	bSizerGrid = new wxFlexGridSizer( 2, 0, 0 );
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
	wxTextCtrl * control = (wxTextCtrl*) FindWindowById(id);
	return control ? control->GetValue() : wxString(wxEmptyString);
}

void FbAuthorModifyDlg::SetValue(wxWindowID id, const wxString &text)
{
	wxTextCtrl * control = (wxTextCtrl*) FindWindowById(id);
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
		wxString sql = wxT("UPDATE books SET id_author=? WHERE id_author=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, new_id);
		stmt.Bind(2, old_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id) WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, new_id);
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

BEGIN_EVENT_TABLE( FbAuthorReplaceDlg, wxDialog )
	EVT_TEXT_ENTER( ID_FIND_TXT, FbAuthorReplaceDlg::OnFindEnter )
	EVT_BUTTON( ID_FIND_BTN, FbAuthorReplaceDlg::OnFindEnter )
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbAuthorReplaceDlg::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbAuthorReplaceDlg::OnArray)
END_EVENT_TABLE()

FbAuthorReplaceDlg::FbAuthorReplaceDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
		m_id(id), m_thread(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizerGrid;
	fgSizerGrid = new wxFlexGridSizer( 2, 0, 0 );
	fgSizerGrid->AddGrowableCol( 1 );
	fgSizerGrid->SetFlexibleDirection( wxBOTH );
	fgSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * m_static1 = new wxStaticText( this, wxID_ANY, _("Replace") + COLON, wxDefaultPosition, wxDefaultSize, 0 );
	m_static1->Wrap( -1 );
	fgSizerGrid->Add( m_static1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_Text = new wxTextCtrl( this, ID_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_Text->SetMinSize( wxSize( 300,-1 ) );

	fgSizerGrid->Add( m_Text, 0, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	wxStaticText * m_static2 = new wxStaticText( this, wxID_ANY, _("Find") + COLON, wxDefaultPosition, wxDefaultSize, 0 );
	m_static2->Wrap( -1 );
	fgSizerGrid->Add( m_static2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bSizerFind;
	bSizerFind = new wxBoxSizer( wxHORIZONTAL );

	m_FindText = new wxTextCtrl( this, ID_FIND_TXT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 250,-1 ) );

	bSizerFind->Add( m_FindText, 1, wxALIGN_CENTER_VERTICAL, 5 );

	m_FindBtn = new wxBitmapButton( this, ID_FIND_BTN, wxArtProvider::GetBitmap(wxART_FIND), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerFind->Add( m_FindBtn, 0, wxRIGHT|wxLEFT, 5 );

	fgSizerGrid->Add( bSizerFind, 0, wxEXPAND, 5 );

	bSizerMain->Add( fgSizerGrid, 0, wxEXPAND, 5 );

	m_FindList = new FbTreeViewCtrl(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	bSizerMain->Add( m_FindList, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	m_FindList->AddColumn(0, _("Author"), 40, wxALIGN_LEFT);
	m_FindList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);

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
		if (!last_name.IsEmpty()) (m_thread = new FbAuthListThread(this, last_name))->Execute();
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
	if (!text.IsEmpty()) (m_thread = new FbAuthListThread(this, text))->Execute();
}

int FbAuthorReplaceDlg::GetSelected()
{
	FbModelItem item = m_FindList->GetCurrent();
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
	return m_FindList->GetCurrent();
}

int FbAuthorReplaceDlg::Execute(int author, wxString& newname)
{
	FbAuthorReplaceDlg dlg(_("Replace author"), author);
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
	m_FindList->AssignModel(model);
}

void FbAuthorReplaceDlg::OnArray( FbArrayEvent& event )
{
	wxArrayInt array = event.GetArray();
	int index = array.Index(m_id);
	if (index != wxNOT_FOUND) array.RemoveAt(index);

	FbAuthListModel * model = wxDynamicCast(m_FindList->GetModel(), FbAuthListModel);
	if (model) model->Append(array);
	m_FindList->Refresh();
}

bool FbAuthorReplaceDlg::Delete(FbModel &model)
{
	FbModelItem item = model.GetCurrent();
	FbAuthListData * current = wxDynamicCast(&item, FbAuthListData);
	if (current == NULL) return false;

	int id = current->GetCode();
	int count = 0;
	{
		wxString sql = wxT("SELECT COUNT(DISTINCT id) FROM books WHERE id_author=?");
		FbCommonDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		count = result.NextRow() ? result.GetInt(0) : 0;
	}

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
