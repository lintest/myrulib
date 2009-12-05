#include "FbAuthorDlg.h"
#include "FbConst.h"

FbAuthorDlg::FbAuthorDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* bSizerGrid;
	bSizerGrid = new wxFlexGridSizer( 2, 2, 0, 0 );
	bSizerGrid->AddGrowableCol( 1 );
	bSizerGrid->SetFlexibleDirection( wxBOTH );
	bSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	AppenName(bSizerGrid, ID_LAST_NAME,   _("Фамилия"))->SetFocus();
	AppenName(bSizerGrid, ID_FIRST_NAME,  _("Имя"));
	AppenName(bSizerGrid, ID_MIDDLE_NAME, _("Отчество"));

	bSizerMain->Add( bSizerGrid, 1, wxEXPAND, 5 );

	wxStdDialogButtonSizer * m_sdbSizerBtn = new wxStdDialogButtonSizer();
	wxButton * m_sdbSizerBtnOK = new wxButton( this, wxID_OK );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnOK );
	m_sdbSizerBtnOK->SetDefault();
	wxButton * m_sdbSizerBtnCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerBtn->AddButton( m_sdbSizerBtnCancel );
	m_sdbSizerBtn->Realize();
	bSizerMain->Add( m_sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	wxSize newSize = GetBestSize();
	this->SetSize(newSize);
}

wxTextCtrl * FbAuthorDlg::AppenName(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption)
{
	wxStaticText * text = new wxStaticText( this, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, 0 );
	text->Wrap( -1 );
	parent->Add( text, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl * edit = new wxTextCtrl( this, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	edit->SetMinSize( wxSize( 300,-1 ) );
	parent->Add( edit, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	return edit;
}

int FbAuthorDlg::Append()
{
	FbAuthorDlg dlg(_("Добавить автора"));
	bool ok = dlg.ShowModal() == wxID_OK;
	return ok ? dlg.DoAppend() : 0;
}

int FbAuthorDlg::Modify(int id)
{
	FbAuthorDlg dlg(_("Изменить автора"), id);
	bool ok = dlg.Load(id) && dlg.ShowModal() == wxID_OK;
	return ok ? dlg.DoUpdate() : 0;
}

bool FbAuthorDlg::Load(int id)
{
	AuthorItem author(id);
	bool ok = author.Load(m_database);
	SetValue(ID_LAST_NAME, author.last);
	SetValue(ID_FIRST_NAME, author.first);
	SetValue(ID_MIDDLE_NAME, author.middle);
	return ok;
}

wxString FbAuthorDlg::GetValue(wxWindowID id)
{
	wxTextCtrl * control = (wxTextCtrl*) FindWindowById(id);
	return control ? control->GetValue() : wxString(wxEmptyString);
}

void FbAuthorDlg::SetValue(wxWindowID id, const wxString &text)
{
	wxTextCtrl * control = (wxTextCtrl*) FindWindowById(id);
	if (control) control->SetValue(text);
}

int FbAuthorDlg::FindAuthor()
{
	AuthorItem author(m_id);
	GetValues(author);
	return author.Find(m_database);
}

int FbAuthorDlg::DoAppend()
{
	AuthorItem author;
	GetValues(author);
	return author.Save(m_database);
}

int FbAuthorDlg::DoUpdate()
{
	if (m_exists) {
		ReplaceAuthor(m_id, m_exists);
		return m_exists;
	} else {
		DoModify();
		return m_id;
	}
}

void FbAuthorDlg::GetValues(AuthorItem &author)
{
	author.last   = GetValue(ID_LAST_NAME).Trim(false).Trim(true);
	author.first  = GetValue(ID_FIRST_NAME).Trim(false).Trim(true);
	author.middle = GetValue(ID_MIDDLE_NAME).Trim(false).Trim(true);
}

void FbAuthorDlg::DoModify()
{
	AuthorItem author(m_id);
	GetValues(author);
	author.Save(m_database);
}

void FbAuthorDlg::ReplaceAuthor(int old_id, int new_id)
{
	FbCommonDatabase m_database;
	wxSQLite3Transaction trans(&m_database, WXSQLITE_TRANSACTION_DEFERRED);

	{
		wxString sql = wxT("UPDATE books SET id_author=? WHERE id_author=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, new_id);
		stmt.Bind(2, old_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = strUpdateAuthorCount + wxT("WHERE id=?");
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

	trans.Commit();
}

void FbAuthorDlg::EndModal(int retCode)
{
	if ( retCode == wxID_OK) {
		if (GetValue(ID_LAST_NAME).IsEmpty()) {
			wxMessageBox(_("Не заполнена фамилия автора."), GetTitle());
			return;
		}
		m_exists = FindAuthor();
		if (m_exists) {
			wxString msg = _("Автор с такими реквизитами уже существует.");
			wxString title = GetTitle() + wxT("…");
			if (m_id) {
				msg += _("\nОбъединить двух авторов?");
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
