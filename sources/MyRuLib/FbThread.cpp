#include "FbThread.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbThread
//-----------------------------------------------------------------------------

wxCriticalSection FbThread::sm_section;

bool FbThread::IsClosed()
{
	wxCriticalSectionLocker locker(sm_section);
	return m_closed;
}

void FbThread::Close()
{
	wxCriticalSectionLocker locker(sm_section);
	m_closed = true;
}


//-----------------------------------------------------------------------------
//  FbProgressThread
//-----------------------------------------------------------------------------

wxCriticalSection FbProgressThread::sm_queue;

void FbProgressThread::DoStart(const wxString & msg, int max)
{
	m_text = m_info + wxT(' ') + msg;
	m_max = max;
	m_pos = 0;

	FbProgressEvent(ID_PROGRESS_UPDATE, m_text).Post(m_owner);
}

void FbProgressThread::DoStep(const wxString & msg)
{
	int pos = m_max ? (++m_pos * 1000 / m_max) : 0;
	FbProgressEvent(ID_PROGRESS_UPDATE, m_text, pos, msg).Post(m_owner);
}

void FbProgressThread::DoFinish()
{
	FbProgressEvent(ID_PROGRESS_UPDATE).Post(m_owner);
}

