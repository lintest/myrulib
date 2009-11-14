#include "BaseThread.h"
#include "MyRuLibApp.h"
#include "FbConst.h"
#include "FbParams.h"

wxCriticalSection BaseThread::sm_queue;

wxString BaseThread::CalcMd5(md5_context &md5)
{
	wxString md5sum;
	unsigned char output[16];
	md5_finish( &md5, output );
	for (size_t i=0; i<16; i++)
		md5sum += wxString::Format(wxT("%02x"), output[i]);
	return md5sum;
}

void BaseThread::DoStart(const int max, const wxString & msg)
{
	m_text = m_info + wxT(" ") + msg;
	m_progress = 0;
	m_max = max;

	wxUpdateUIEvent event( ID_PROGRESS_START );
	event.SetText(m_text);
	event.SetString(m_text);
	event.SetInt(1000);
	wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoStep(const wxString & msg)
{
	int pos = 0;
	if (m_max) {
		m_progress++;
		pos = m_progress * 1000 / m_max;
	}

	wxUpdateUIEvent event( ID_PROGRESS_UPDATE );
	event.SetText(m_text);
	event.SetString(msg);
	event.SetInt(pos);
	wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoFinish()
{
	wxUpdateUIEvent event( ID_PROGRESS_FINISH );
	wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::UpdateBooksCount(wxSQLite3Database &database)
{
	database.ExecuteUpdate(strUpdateCountSQL);
}
