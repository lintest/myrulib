#include "BaseThread.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbParams.h"

wxCriticalSection BaseThread::sm_queue;

void BaseThread::DoStart(const int max, const wxString & msg)
{
	m_text = m_info + wxT(" ") + msg;
	m_pos = 0;
	m_max = max;

	FbProgressEvent(ID_PROGRESS_UPDATE, m_text).Post();
}

void BaseThread::DoStep(const wxString & msg)
{
	int pos = m_max ? (++m_pos * 1000 / m_max) : 0;
	FbProgressEvent(ID_PROGRESS_UPDATE, m_text, pos, msg).Post();
}

void BaseThread::DoFinish()
{
	FbProgressEvent(ID_PROGRESS_UPDATE).Post();
}

