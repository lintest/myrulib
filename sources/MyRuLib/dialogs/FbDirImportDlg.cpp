#include "FbDirImportDlg.h"
#include <wx/artprov.h>
#include <wx/wxsqlite3.h>
#include "FbDataPath.h"
#include "FbLogoBitmap.h"
#include "FbDatabase.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbProgressDlg.h"
#include "FbImportThread.h"
#include "MyRuLibApp.h"

BEGIN_EVENT_TABLE( FbDirImportDlg, FbDialog )
	EVT_BUTTON( ID_FOLDER, FbDirImportDlg::OnSelectFolderClick )
END_EVENT_TABLE()

FbDirImportDlg::FbDirImportDlg( wxWindow* parent, const wxString &title )
	: FbDialog( parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Select folder to recursive import"), wxDefaultPosition, wxDefaultSize, 0 );
	info->Wrap( -1 );
	bSizerMain->Add( info, 0, wxTOP|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_folder.Create( this, ID_FOLDER, wxGetApp().GetLibPath() );
	bSizerMain->Add( &m_folder, 0, wxEXPAND|wxALL, 5 );

	m_only_new.Create( this, wxID_ANY, _("Process only new files") );
	m_only_new.SetValue(false);
	bSizerMain->Add( &m_only_new, 0, wxALL|wxEXPAND, 5 );

	m_only_md5.Create( this, wxID_ANY, _("Exclude files missing from the collection") );
	m_only_md5.SetValue(false);
	bSizerMain->Add( &m_only_md5, 0, wxALL|wxEXPAND, 5 );

	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	bSizerMain->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	m_folder.SetFocus();
}

void FbDirImportDlg::OnSelectFolderClick( wxCommandEvent& event )
{
	wxDirDialog dlg(
		this,
		_("Select folder"),
		m_folder.GetValue(),
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON
	);
	if (dlg.ShowModal() == wxID_OK) {
		m_folder.SetValue(dlg.GetPath());
	}
}

wxString FbDirImportDlg::GetDirname()
{
	return m_folder.GetValue();
}

FbThread * FbDirImportDlg::CreateThread(wxEvtHandler * owner)
{
	long flags = fbIMP_IMPORT;
	if (m_only_new.GetValue()) flags = flags | fbIMP_ONLY_NEW;
	if (m_only_md5.GetValue()) flags = flags | fbIMP_ONLY_MD5;
	return new FbDirImportThread(owner, GetDirname(), flags);
}

bool FbDirImportDlg::Execute(wxWindow * parent)
{
	FbDirImportDlg dlg(parent, _("Add folder"));
	if (dlg.ShowModal() != wxID_OK) return wxEmptyString;

	FbProgressDlg scaner(dlg.GetParent(), _("Add folder"));
	scaner.RunThread(dlg.CreateThread(&scaner));
	return (scaner.ShowModal() == wxID_OK);
}

