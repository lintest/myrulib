#include "FbProgressDlg.h"
#include "FbConst.h"
#include "FbLogoBitmap.h"

//-----------------------------------------------------------------------------
//  FbDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbProgressDlg, FbDialog)
	EVT_TIMER(wxID_ANY, FbProgressDlg::OnTimer)
	EVT_FB_PROGRESS(ID_PROGRESS_START,  FbProgressDlg::OnGaugeStart)
	EVT_FB_PROGRESS(ID_PROGRESS_UPDATE, FbProgressDlg::OnGaugeUpdate)
	EVT_FB_PROGRESS(ID_PROGRESS_PULSE, FbProgressDlg::OnGaugePulse)
END_EVENT_TABLE()


FbProgressDlg::FbProgressDlg(wxWindow* parent)
	: FbDialog( parent, wxID_ANY, _("Processing collection"), wxDefaultPosition, wxDefaultSize),
		m_thread(NULL), m_timer(this)
{
	wxBoxSizer * bSizerMain = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer * bSizerCtrl = new wxBoxSizer( wxVERTICAL );

	wxStaticBitmap * m_bitmap = new wxStaticBitmap( this, wxID_ANY, FbLogoBitmap(), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_bitmap, 0, wxALL|wxALIGN_TOP, 10 );

	m_text.Create( this, wxID_ANY, wxEmptyString );
	m_text.Wrap( -1 );
	bSizerCtrl->Add( &m_text, 0, wxALL|wxEXPAND, 5 );

	m_gauge.Create(this, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	m_gauge.SetMinSize(wxSize(300, -1));
	bSizerCtrl->Add( &m_gauge, 0, wxALL|wxEXPAND, 5 );

	wxStdDialogButtonSizer * buttons = CreateStdDialogButtonSizer( wxCANCEL );
	bSizerCtrl->Add( buttons, 0, wxEXPAND|wxALL, 5 );

	bSizerMain->Add( bSizerCtrl, 1, wxEXPAND);

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

void FbProgressDlg::OnGaugeStart(FbProgressEvent & event)
{
	m_timer.Stop();
	m_text.SetLabel(event.m_str);
	m_gauge.SetRange(event.m_pos);
}

void FbProgressDlg::OnGaugeUpdate(FbProgressEvent & event)
{
	m_gauge.SetValue(event.m_pos);
}

void FbProgressDlg::OnGaugePulse(FbProgressEvent & event)
{
	m_text.SetLabel(event.m_str);
	m_timer.Start(100);
}
