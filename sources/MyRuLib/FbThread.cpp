#include "FbThread.h"
#include "FbBookEvent.h"
#include "FbConst.h"

//-----------------------------------------------------------------------------
//  FbThread
//-----------------------------------------------------------------------------

wxCriticalSection FbThread::sm_section;

void FbThread::DeleteRef(PFbThread & thread)
{
	if (thread) {
		thread->Close();
		thread->Wait();
		delete thread;
		thread = NULL;
	}
}

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

void FbProgressThread::DoPulse(const wxString & msg)
{
	FbProgressEvent(ID_PROGRESS_PULSE, msg).Post(m_owner);
}

void FbProgressThread::DoStart(const wxString & msg, int max)
{
	wxString text = msg;
	if (!m_info.IsEmpty()) {
		text.Prepend(m_info + wxT(' '));
	}
	m_max = max;
	m_pos = 0;

	FbProgressEvent(ID_PROGRESS_START, text, 1000).Post(m_owner);
}

void FbProgressThread::DoStep(const wxString & msg)
{
	int pos = m_max ? (++m_pos * 1000 / m_max) : 0;
	FbProgressEvent(ID_PROGRESS_UPDATE, msg, pos).Post(m_owner);
}

void FbProgressThread::DoFinish()
{
	FbProgressEvent(ID_PROGRESS_UPDATE).Post(m_owner);
}

