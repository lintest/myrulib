#include "FbThread.h"

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
