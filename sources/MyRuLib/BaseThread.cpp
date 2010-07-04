#include "BaseThread.h"
#include "FbBookEvent.h"
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

	FbProgressEvent(ID_PROGRESS_UPDATE, m_text).Post();
}

void BaseThread::DoStep(const wxString & msg)
{
	int pos = m_max ? (++m_progress * 1000 / m_max) : 0;
	FbProgressEvent(ID_PROGRESS_UPDATE, m_text, pos, msg).Post();
}

void BaseThread::DoFinish()
{
	FbProgressEvent(ID_PROGRESS_UPDATE).Post();
}

