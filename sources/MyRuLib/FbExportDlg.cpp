#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/arrimpl.cpp>
#include "FbExportDlg.h"
#include "FbParams.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbExportTree.h"
#include "FbBookPanel.h"
#include "FbConvertDlg.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbExportDlg, wxDialog )
	EVT_BUTTON( ID_DIR_BTN, FbExportDlg::OnSelectDir )
	EVT_CHOICE( wxID_ANY, FbExportDlg::OnChangeFormat )
	EVT_CHECKBOX( ID_AUTHOR, FbExportDlg::OnCheckAuthor )
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, const wxString & selections, int iAuthor) :
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

	if (iAuthor) {
		m_checkAuthor = new wxCheckBox( this, ID_AUTHOR, _("Use Author (without co-Authors)"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerMain->Add( m_checkAuthor, 0, wxALL, 5 );
		m_checkAuthor->SetValue(1);
	}

	m_books = new FbTreeViewCtrl( this, ID_BOOKS, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_VRULES);
	m_books->SetMinSize( wxSize( -1,250 ) );
	m_books->AddColumn (0, _("File name"), 5, wxALIGN_LEFT);
	m_books->AddColumn (1, _("Size, Kb"), 1, wxALIGN_RIGHT);
	m_books->AssignModel(new FbExportTreeModel(m_selections, m_author));

	bSizerMain->Add( m_books, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat;
	bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextFormat = new wxStaticText( this, wxID_ANY, _("Export As..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFormat->Wrap( -1 );
	bSizerFormat->Add( m_staticTextFormat, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_format = new FbChoiceInt( this, ID_FORMAT, wxDefaultPosition, wxDefaultSize);
	bSizerFormat->Add( m_format, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerFormat, 0, wxEXPAND, 5 );

	m_textDir->SetValue( FbParams::GetPath(FB_EXTERNAL_DIR) );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	SetAffirmativeId(wxID_OK);
	SetEscapeId(wxID_CANCEL);

	LoadFormats();
}

FbExportDlg::~FbExportDlg()
{
}

void FbExportDlg::LoadFormats()
{
	wxString filename = _("filename");
	int format = FbParams::GetInt(FB_FILE_FORMAT);
	m_format->Append(filename << wxT(".fb2"), 0);
	m_format->Append(filename + wxT(".zip"), -1);
	m_format->Append(filename + wxT(".gz"),  -2);
	m_format->Append(filename + wxT(".bz2"), -3);

	int index;
	switch (format) {
		case -1: index = 1; break;
		case -2: index = 2; break;
		case -3: index = 3; break;
		default: index = 0; break;
	}
	m_format->SetSelection(index);

	wxString sql = wxT("SELECT id, name FROM script ORDER BY id");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while ( result.NextRow() ) {
		int code = result.GetInt(0);
		wxString name = _("filename"); name << wxT('.') << result.GetString(1);
		int index = m_format->Append(name, code);
		if (code == format) m_format->SetSelection(index);
	}
	ChangeFormat();
}

wxString FbExportDlg::GetExt(int format)
{
	if (format > 0) {
		wxString sql = wxT("SELECT name FROM script WHERE id=?");
		FbLocalDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, format);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetString(0);
	}

	return wxEmptyString;
}

void FbExportDlg::OnSelectDir( wxCommandEvent& event )
{
	wxDirDialog dlg(
		this,
		_("Select a destination folder"),
		m_textDir->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK) m_textDir->SetValue(dlg.GetPath());
}

void FbExportDlg::OnChangeFormat( wxCommandEvent& event )
{
	ChangeFormat();
}

void FbExportDlg::ChangeFormat()
{
	FbExportTreeModel * model = wxDynamicCast(m_books->GetModel(), FbExportTreeModel);
	if (!model) return;

	int format = m_format->GetCurrentData();
	int scale = format < 0 ? 43 : 100;
	wxString arc, ext;
	switch (format) {
		case -1: arc = wxT("zip"); break;
		case -2: arc = wxT("gz"); break;
		case -3: arc = wxT("bz2"); break;
	}

	model->SetFormat(GetExt(format), arc, scale);
	m_books->Refresh();
}

bool FbExportDlg::ExportBooks()
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

	FbExportTreeModel * model = wxDynamicCast(m_books->GetModel(), FbExportTreeModel);
	if (!model) return false;

	FbConvertDlg * dlg = new FbConvertDlg(wxGetApp().GetTopWindow(), wxID_ANY, wxT("Export files"));
	model->GetFiles(dlg->m_filelist);
	dlg->m_root = m_textDir->GetValue();
	dlg->SetSize(GetSize());
	dlg->SetPosition(GetPosition());
	dlg->m_format = m_format->GetCurrentData();
	dlg->Execute();

	return true;
}

bool FbExportDlg::Execute(wxWindow* parent, FbBookPanel * books, int iAuthor)
{
	wxString selections = books->GetSelected();

	if ( selections.IsEmpty() ) {
		wxMessageBox(_("Not selected any book"));
		return false;
	}

	FbExportDlg dlg(parent, selections, iAuthor);
	return (dlg.ShowModal() == wxID_OK) && dlg.ExportBooks();
}

void FbExportDlg::OnCheckAuthor( wxCommandEvent& event )
{
	int author = 0;
	if ( m_checkAuthor && m_checkAuthor->GetValue() ) author = m_author;
	m_books->AssignModel(new FbExportTreeModel(m_selections, author));
	ChangeFormat();
}
