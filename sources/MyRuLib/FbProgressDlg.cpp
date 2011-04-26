#include "FbProgressDlg.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbProgressDlg, FbDialog)
	EVT_TIMER(wxID_ANY, FbProgressDlg::OnTimer)
	EVT_FB_PROGRESS(ID_PROGRESS_UPDATE, FbProgressDlg::OnProgress)
	EVT_FB_PROGRESS(ID_PROGRESS_PULSE, FbProgressDlg::OnPulseGauge)
END_EVENT_TABLE()


FbProgressDlg::FbProgressDlg(wxWindow* parent)
	: FbDialog( parent, wxID_ANY, _("Processing collection"), wxDefaultPosition, wxDefaultSize),
		m_thread(NULL), m_timer(this)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_text.Create( this, wxID_ANY, wxEmptyString );
	m_text.Wrap( -1 );
	bSizerMain->Add( &m_text, 0, wxALL|wxEXPAND, 5 );

	m_gauge.Create( this, wxID_ANY, 100 );
	m_gauge.SetMinSize( wxSize(300, -1) );
	m_gauge.SetRange(1000);
	bSizerMain->Add( &m_gauge, 0, wxALL|wxEXPAND, 5 );

	wxStdDialogButtonSizer * buttons = CreateStdDialogButtonSizer( wxCANCEL );
	bSizerMain->Add( buttons, 0, wxEXPAND|wxALL, 5 );

	SetSizer( bSizerMain );
	Layout();
	SetSize(GetBestSize());

	m_timer.Start(100);
}

FbProgressDlg::~FbProgressDlg(void)
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbProgressDlg::RunThread(FbThread * thread)
{
	m_thread = thread;
	m_thread->Execute();
}

void FbProgressDlg::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	m_gauge.Pulse();
}

void FbProgressDlg::OnProgress(FbProgressEvent & event)
{
	m_timer.Stop();
	m_text.SetLabel(event.GetString());
	m_gauge.SetValue(event.GetInt());
}

void FbProgressDlg::OnPulseGauge(FbProgressEvent & event)
{
	m_text.SetLabel(event.GetString());
	m_timer.Start(100);
}
