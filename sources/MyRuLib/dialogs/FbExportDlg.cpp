#include <wx/filename.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/arrimpl.cpp>
#include "FbExportDlg.h"
#include "FbParams.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "models/FbExportTree.h"
#include "FbBookPanel.h"
#include "FbConvertDlg.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( FbExportDlg, wxDialog )
	EVT_BUTTON( ID_DIR_TXT, FbExportDlg::OnSelectDir )
	EVT_CHOICE( wxID_ANY, FbExportDlg::OnChangeFormat )
	EVT_CHECKBOX( ID_AUTHOR, FbExportDlg::OnCheckAuthor )
	EVT_CHECKBOX( ID_DIR, FbExportDlg::OnChangeFormat )
	EVT_CHECKBOX( ID_FILE, FbExportDlg::OnChangeFormat )
	EVT_TEXT( ID_STRUCT, FbExportDlg::OnCheckAuthor )
END_EVENT_TABLE()

FbExportDlg::FbExportDlg( wxWindow* parent, const wxString & selections, int iAuthor) :
	FbDialog( parent, wxID_ANY, _("Export to external storage"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
	m_selections(selections),
	m_author(iAuthor),
	m_checkAuthor(NULL)
{
	SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer * fgSizerTop = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop->AddGrowableCol( 1 );
	fgSizerTop->SetFlexibleDirection( wxBOTH );
	fgSizerTop->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText * folder_info = new wxStaticText( this, wxID_ANY, _("Destination folder:"));
	folder_info->Wrap( -1 );
	fgSizerTop->Add( folder_info, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	m_folder = new FbCustomCombo(this, ID_DIR_TXT);
	m_folder->SetMinSize( wxSize( 300,-1 ) );
	fgSizerTop->Add( m_folder, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	wxStaticText * format_info = new wxStaticText( this, wxID_ANY, _("Exported structure:"));
	format_info->Wrap( -1 );
	fgSizerTop->Add( format_info, 0, wxTOP|wxLEFT|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	wxString structure = FbParams(FB_FOLDER_FORMAT).Str();

	m_struct = new wxComboBox( this, ID_STRUCT, structure, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_folder->SetMinSize( wxSize( 300,-1 ) );
	m_struct->Append( wxT("%a/%f/%s/%n %t") );
	m_struct->Append( wxT("%a/%f/%t") );
	m_struct->Append( wxT("%f/%s/%n %t") );
	m_struct->Append( wxT("%f/%s/%t") );
	m_struct->Append( wxT("%f/%m") );
	fgSizerTop->Add( m_struct, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( fgSizerTop, 0, wxEXPAND, 5 );

	wxBoxSizer * bSizerTrans = new wxBoxSizer( wxHORIZONTAL );

	m_transDir = new wxCheckBox( this, ID_DIR, _("Transliterate folder name"));
	bSizerTrans->Add( m_transDir, 1, wxALL|wxEXPAND, 5 );

	m_transFile = new wxCheckBox( this, ID_FILE, _("Transliterate filename"));
	bSizerTrans->Add( m_transFile, 1, wxALL|wxEXPAND, 5 );

	bSizerMain->Add( bSizerTrans, 0, wxEXPAND, 5 );

	if (iAuthor) {
		m_checkAuthor = new wxCheckBox( this, ID_AUTHOR, _("Use Author (without co-Authors)"));
		bSizerMain->Add( m_checkAuthor, 0, wxALL, 5 );
		m_checkAuthor->SetValue(1);
	}

	m_books = new FbTreeViewCtrl( this, ID_BOOKS, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | fbTR_VRULES);
	m_books->SetMinSize( wxSize( -1,250 ) );
	m_books->AddColumn (0, _("File name"), -10, wxALIGN_LEFT);
	m_books->AddColumn (1, _("Size, Kb"), 6, wxALIGN_RIGHT);

	bSizerMain->Add( m_books, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFormat;
	bSizerFormat = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText * m_staticTextFormat = new wxStaticText( this, wxID_ANY, _("Export As..."));
	m_staticTextFormat->Wrap( -1 );
	bSizerFormat->Add( m_staticTextFormat, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_format = new FbChoiceInt( this, ID_FORMAT, wxDefaultPosition, wxDefaultSize);
	bSizerFormat->Add( m_format, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bSizerMain->Add( bSizerFormat, 0, wxEXPAND, 5 );

	m_folder->SetValue( FbParamItem::GetPath(FB_EXTERNAL_DIR) );
	m_transDir->SetValue( FbParams(FB_TRANSLIT_FOLDER) );
	m_transFile->SetValue( FbParams(FB_TRANSLIT_FILE) );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	SetAffirmativeId(wxID_OK);
	SetEscapeId(wxID_CANCEL);

	LoadFormats();

	m_books->AssignModel(CreateModel());
}

FbExportDlg::~FbExportDlg()
{
}

void FbExportDlg::LoadFormats()
{
	wxString filename = _("filename");
	int format = FbParams(FB_FILE_FORMAT);
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
}

wxString FbExportDlg::GetExt(int format)
{
	return format > 0 ? FbLocalDatabase().Str(format, wxT("SELECT name FROM script WHERE id=?")) : wxString();
}

void FbExportDlg::OnSelectDir( wxCommandEvent& event )
{
	wxComboCtrl * control = wxDynamicCast(FindWindow(event.GetId()), wxComboCtrl);
	if (!control) return;

	wxDirDialog dlg(
		this,
		_("Select folder"),
		control->GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);

	if (dlg.ShowModal() == wxID_OK) control->SetValue(dlg.GetPath());
}

void FbExportDlg::OnChangeFormat( wxCommandEvent& event )
{
	if (FbExportTreeModel * model = wxDynamicCast(m_books->GetModel(), FbExportTreeModel))  {
		ChangeFormat(model);
		model->Create();
		m_books->Refresh();
	}
}

void FbExportDlg::ChangeFormat(FbExportTreeModel * model)
{
	int format = m_format->GetValue();
	int scale = format < 0 ? 43 : 100;
	wxString arc, ext;
	switch (format) {
		case -1: arc = wxT("zip"); break;
		case -2: arc = wxT("gz"); break;
		case -3: arc = wxT("bz2"); break;
	}
	model->SetFormat(GetExt(format), arc, scale);
	model->SetTransDir(m_transDir->GetValue());
	model->SetTransFile(m_transFile->GetValue());
}

bool FbExportDlg::ExportBooks()
{
	wxString root_dir = m_folder->GetValue();
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
	dlg->m_root = m_folder->GetValue();
	dlg->SetSize(GetSize());
	dlg->SetPosition(GetPosition());
	dlg->m_format = m_format->GetValue();
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
	m_books->AssignModel(CreateModel());
}

FbModel * FbExportDlg::CreateModel()
{
	int author = 0;
	if ( m_checkAuthor && m_checkAuthor->GetValue() ) author = m_author;
	FbExportTreeModel * model = new FbExportTreeModel(m_selections, m_struct->GetValue(), author);
	ChangeFormat(model);
	model->Create();
	return model;
}
