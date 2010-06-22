#include "FbThread.h"

wxCriticalSection FbMutex::sm_section;

IMPLEMENT_CLASS(FbMutexLocker, wxObject)

IMPLEMENT_CLASS(FbMutexTester, wxObject)

IMPLEMENT_CLASS(FbThreadSkiper, wxObject)

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
