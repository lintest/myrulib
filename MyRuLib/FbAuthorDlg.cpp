#include "FbAuthorDlg.h"
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include "FbDatabase.h"

FbAuthorDlg::FbAuthorDlg( const wxString& title, int id )
	: FbDialog( NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ), m_id(id)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* bSizerGrid;
	bSizerGrid = new wxFlexGridSizer( 2, 2, 0, 0 );
	bSizerGrid->AddGrowableCol( 1 );
	bSizerGrid->SetFlexibleDirection( wxBOTH );
	bSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	AppenName(bSizerGrid, ID_LAST_NAME,   _("Фамилия"));
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

FbAuthorDlg::~FbAuthorDlg()
{
}

void FbAuthorDlg::AppenName(wxFlexGridSizer * parent, wxWindowID id, const wxString &caption)
{
	wxStaticText * text = new wxStaticText( this, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, 0 );
	text->Wrap( -1 );
	parent->Add( text, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl * edit = new wxTextCtrl( this, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	edit->SetMinSize( wxSize( 300,-1 ) );
	parent->Add( edit, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
}

bool FbAuthorDlg::Append()
{
	FbAuthorDlg dlg(_("Добавить автора"));
	bool result = dlg.ShowModal() == wxID_OK;
	return result;
}

bool FbAuthorDlg::Modify(int id)
{
	FbAuthorDlg dlg(_("Изменить автора"), id);
	bool ok = dlg.Load(id) && dlg.ShowModal() == wxID_OK;
	return ok;
}

bool FbAuthorDlg::Load(int id)
{
	wxString sql = wxT("SELECT last_name, first_name, middle_name FROM authors WHERE id=?");

	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	bool ok = result.NextRow();
	if ( ok ) {
		SetValue(ID_LAST_NAME, result.GetString(0));
		SetValue(ID_FIRST_NAME, result.GetString(1));
		SetValue(ID_MIDDLE_NAME, result.GetString(2));
	}
	return ok;
}

void FbAuthorDlg::SetValue(wxWindowID id, const wxString &text)
{
	if (wxTextCtrl * control = (wxTextCtrl*)FindWindowById(id)) {
		control->SetValue(text);
	}
}
