#include "FbScanerDlg.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbScanerDlg, FbDialog)
	EVT_MENU(ID_PROGRESS_1, FbScanerDlg::OnProgress1)
	EVT_MENU(ID_PROGRESS_2, FbScanerDlg::OnProgress2)
END_EVENT_TABLE()


FbScanerDlg::FbScanerDlg(wxWindow* parent, const wxFileName &filename, const wxFileName &dirname, bool only_new)
	: FbDialog( parent, wxID_ANY, _("Processing collection"), wxDefaultPosition, wxDefaultSize),
		m_thread(this, filename, dirname, only_new)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, wxEmptyString );
	m_text.Wrap( -1 );
	bSizerMain->Add( &m_text, 0, wxALL|wxEXPAND, 5 );

	m_gauge1.Create( this, wxID_ANY, 100 );
	m_gauge1.SetMinSize( wxSize(300, -1) );
	bSizerMain->Add( &m_gauge1, 0, wxALL|wxEXPAND, 5 );

	m_gauge2.Create( this, wxID_ANY, 100 );
	m_gauge2.SetMinSize( wxSize(300, -1) );
	bSizerMain->Add( &m_gauge2, 0, wxALL|wxEXPAND, 5 );

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
	if (m_thread.IsAlive()) m_thread.Wait();
}

void FbScanerDlg::OnProgress1(wxCommandEvent & event)
{
	m_text.SetLabel(event.GetString());
	m_gauge1.SetValue(event.GetInt());
}

void FbScanerDlg::OnProgress2(wxCommandEvent & event)
{
	m_gauge2.SetValue(event.GetInt());
}
