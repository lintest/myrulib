#define fbMAX_FILENAME_LENGTH 100
/*
static wxString wxFileName::GetForbiddenChars( wxPathFormat format = wxPATH_NATIVE )
Returns the characters that can't be used in filenames and directory names for the specified format.
*/

#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/arrimpl.cpp>
#include "ExternalDlg.h"
#include "FbParams.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbExportTree.h"
#include "FbBookPanel.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ExternalDlg, wxDialog )
	EVT_BUTTON( ID_DIR_BTN, ExternalDlg::OnSelectDir )
	EVT_CHOICE( wxID_ANY, ExternalDlg::OnChangeFormat )
	EVT_CHECKBOX( ID_AUTHOR, ExternalDlg::OnCheckAuthor )
END_EVENT_TABLE()

ExternalDlg::ExternalDlg( wxWindow* parent, const wxString & selections, int iAuthor) :
	FbDialog( parent, wxID_ANY, _("Export to external storage"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
	m_selections(selections),
	m_author(iAuthor),
	m_checkAuthor(NULL)
{
	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerDir;
	bSizerDir = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextDir = new wxStaticText( this, wxID_ANY, _("Destination folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDir->Wrap( -1 );
	bSizerDir->Add( m_staticTextDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_textDir = new wxTextCtrl( this, ID_DIR_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textDir->SetMinSize( wxSize( 300,-1 ) );

	bSizerDir->Add( m_textDir, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxBitmapButton * m_bpButtonDir = new wxBitmapButton( this, ID_DIR_BTN, wxArtProvider::GetBitmap(wxART_FOLDER_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizerDir->Add( m_bpButtonDir, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerDir, 0, wxEXPAND, 5 );

	if (iAuthor != ciNoAuthor) {
		m_checkAuthor = new wxCheckBox( this, ID_AUTHOR, _("Use Author (without co-Authors)"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerMain->Add( m_checkAuthor, 0, wxALL, 5 );
		m_checkAuthor->SetValue(1);
	}

	m_books = new FbTreeViewCtrl( this, ID_BOOKS, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_VRULES);
	m_books->SetMinSize( wxSize( -1,250 ) );
	m_books->AddColumn (0, _("File name"), 4, wxALIGN_LEFT);
	m_books->AddColumn (1, _("Size, Kb"), 1, wxALIGN_RIGHT);

	FbModel * model = new FbExportTreeModel(selections, iAuthor);
	m_books->AssignModel(model);

	bSizerMain->Add( m_books, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat;
	bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextFormat = new wxStaticText( this, wxID_ANY, _("Export As..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFormat->Wrap( -1 );
	bSizerFormat->Add( m_staticTextFormat, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_format = new FbChoiceFormat( this, ID_FORMAT, wxDefaultPosition, wxDefaultSize);
	bSizerFormat->Add( m_format, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerFormat, 0, wxEXPAND, 5 );

	m_textDir->SetValue( FbParams::GetText(FB_EXTERNAL_DIR) );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	SetAffirmativeId(wxID_OK);
	SetEscapeId(wxID_CANCEL);

	LoadFormats();
}

ExternalDlg::~ExternalDlg()
{
}

void ExternalDlg::LoadFormats()
{
	wxString filename = _("filename");
	int format = FbParams::GetValue(FB_FILE_FORMAT);
	m_format->Append(filename << wxT(".fb2"), 0);
	m_format->Append(filename + wxT(".zip"), -1);
	m_format->Append(filename + wxT(".gz"), -2);
	m_format->SetSelection(format == -1 ? 1 : 0);

	wxString sql = wxT("SELECT id, name FROM script ORDER BY id");

	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while ( result.NextRow() ) {
		int code = result.GetInt(0);
		wxString name = _("filename"); name << wxT('.') << result.GetString(1);
		int index = m_format->Append(name, code);
		if (code == format) m_format->SetSelection(index);
	}
}

void ExternalDlg::FillBooks(const wxString &selections)
{
	int format = m_format->GetCurrentData();
	m_scale = format < 0 ? 43 : 100; // при сжатии средний коэффициент 0.43
	switch (format) {
		case -1: m_ext = wxT(".zip"); break;
		case -2: m_ext = wxT(".gz"); break;
		default: m_ext.Empty();
	}
}

void ExternalDlg::OnSelectDir( wxCommandEvent& event )
{
	wxDirDialog dlg(
		this,
		_("Select a destination folder"),
		m_textDir->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK) m_textDir->SetValue(dlg.GetPath());
}

void ExternalDlg::OnChangeFormat( wxCommandEvent& event )
{
	int format = m_format->GetCurrentData();
	m_scale = format < 0 ? 43 : 100; // при сжатии средний коэффициент 0.43
	switch (format) {
		case -1: m_ext = wxT(".zip"); break;
		case -2: m_ext = wxT(".gz"); break;
		default: m_ext.Empty();
	}
	m_filenames.Empty();
}

bool ExternalDlg::ExportBooks()
{
	wxString root_dir = m_textDir->GetValue();
	if (!wxFileName::DirExists(root_dir)) {
		FbMessageBox(_("Destination folder not found"), root_dir);
		return false;
	}
	if (!wxFileName::IsDirWritable(root_dir)) {
		FbMessageBox(_("Unable write files to destination folder"), root_dir);
		return false;
	}
/*
	FbExportDlg * dlg = new FbExportDlg(wxGetApp().GetTopWindow(), wxID_ANY, wxT("Export files"));
	FillFilelist(m_books->GetRootItem(), dlg->m_filelist);
	dlg->SetSize(GetSize());
	dlg->SetPosition(GetPosition());
	dlg->m_format = m_format->GetCurrentData();
	dlg->Execute();
*/

	return true;
}

bool ExternalDlg::Execute(wxWindow* parent, FbBookPanel * books, int iAuthor)
{
	wxString selections = books->GetSelected();

	if ( selections.IsEmpty() ) {
		wxMessageBox(_("Not selected any book"));
		return false;
	}

	ExternalDlg dlg(parent, selections, iAuthor);
	dlg.FillBooks(selections);
	return (dlg.ShowModal() == wxID_OK) && dlg.ExportBooks();
}

void ExternalDlg::OnCheckAuthor( wxCommandEvent& event )
{
	FillBooks(m_selections);
}


