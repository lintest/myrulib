#include "FbSequenDlg.h"
#include "FbCollection.h"
#include "FbConst.h"

FbSequenDlg::FbSequenDlg( const wxString& title, int id )
	: FbDialog ( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, _("Series name:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( &m_text, 0, wxEXPAND|wxALL, 5 );

	m_edit.Create( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_edit.SetMinSize( wxSize( 300,-1 ) );
	bSizerMain->Add( &m_edit, 0, wxEXPAND|wxALL, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();

	wxSize newSize = GetBestSize();
	this->SetSize(newSize);

	m_edit.SetFocus();
}

int FbSequenDlg::Append(wxString &newname)
{
	FbSequenDlg dlg(_("Append series"));
	bool ok = dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetValue();
	return ok ? dlg.DoAppend() : 0;
}

int FbSequenDlg::Modify(int id, wxString &newname)
{
	FbSequenDlg dlg(_("Modify series"), id);
	bool ok = dlg.Load(id) && dlg.ShowModal() == wxID_OK;
	if (ok) newname = dlg.GetValue();
	return ok ? dlg.DoUpdate() : 0;
}

bool FbSequenDlg::Load(int id)
{
	wxString sql = wxT("SELECT value FROM sequences WHERE id=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	bool ok = result.NextRow();
	if (ok) m_edit.SetValue(result.GetString(0));
	return ok;
}

int FbSequenDlg::Find()
{
	wxString sql = wxT("SELECT id FROM sequences WHERE value=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, m_edit.GetValue());
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0) : 0;
}

int FbSequenDlg::DoAppend()
{
	wxString sql = wxT("INSERT INTO sequences(value, id) VALUES (?,?)");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	m_id = - m_database.NewId(DB_NEW_SEQUENCE);
	stmt.Bind(1, GetValue());
	stmt.Bind(2, m_id);
	stmt.ExecuteUpdate();
	return m_id;
}

int FbSequenDlg::DoModify()
{
	wxString sql = wxT("UPDATE sequences SET value=? WHERE id=?");
	wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
	stmt.Bind(1, GetValue());
	stmt.Bind(2, m_id);
	stmt.ExecuteUpdate();

	FbCollection::ResetSeqn(m_id);
	return m_id;
}

int FbSequenDlg::DoReplace()
{
	wxSQLite3Transaction trans(&m_database, WXSQLITE_TRANSACTION_DEFERRED);
	{
		wxString sql = wxT("UPDATE bookseq SET id_seq=? WHERE id_seq=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_exists);
		stmt.Bind(2, m_id);
		stmt.ExecuteUpdate();
	}
	{
		wxString sql = wxT("DELETE FROM sequences WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		stmt.ExecuteUpdate();
	}
	{
		wxString sql = wxT("DELETE FROM fts_seqn WHERE docid=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		stmt.ExecuteUpdate();
	}
	trans.Commit();

	FbCollection::ResetSeqn(m_exists);
	FbCollection::ResetSeqn(m_id);
	return m_exists;
}

int FbSequenDlg::DoUpdate()
{
	return m_exists ? DoReplace() : DoModify();
}

void FbSequenDlg::EndModal(int retCode)
{
	if ( retCode == wxID_OK) {
		if (GetValue().IsEmpty()) {
			wxMessageBox(_("\"Series name\" field is empty"), GetTitle());
			return;
		}
		m_exists = Find();
		if (m_exists) {
			wxString msg = _("Series aleready exists");
			wxString title = GetTitle() + wxT("...");
			if (m_id) {
				msg += _("Merge series?");
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
