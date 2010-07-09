#include "FbScanerDlg.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbScanerDlg, FbDialog)
	EVT_MENU(ID_PROGRESS_UPDATE, FbScanerDlg::OnProgress)
END_EVENT_TABLE()


FbScanerDlg::FbScanerDlg(const wxFileName &filename, const wxFileName &dirname)
	: FbDialog( NULL, wxID_ANY, wxT("Processing collections"), wxDefaultPosition, wxDefaultSize),
		m_thread(this, filename, dirname)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	m_text.Create( this, wxID_ANY, wxEmptyString );
	m_text.Wrap( -1 );
	bSizerMain->Add( &m_text, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge.Create( this, wxID_ANY, 100 );
	m_gauge.SetMinSize( wxSize(-1, 200) );
	bSizerMain->Add( &m_gauge, 0, wxALL|wxEXPAND, 5 );
	
	wxStdDialogButtonSizer * buttons = CreateStdDialogButtonSizer( wxCANCEL );
	bSizerMain->Add( buttons, 0, wxEXPAND|wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();
	SetSize(GetBestSize());

	m_thread.Execute();
}

FbScanerDlg::~FbScanerDlg(void)
{
	m_thread.Close();
	m_thread.Wait();
}

void FbScanerDlg::OnProgress(wxCommandEvent & event)
{
	m_text.SetLabel(event.GetString());
	m_gauge.Pulse();
}
