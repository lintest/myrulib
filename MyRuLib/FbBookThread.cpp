#include "FbBookThread.h"
#include "InfoCash.h"

void FbBookThread::UpdateInfo()
{
	if (!m_id) return;
	wxThread *thread = new ShowThread(this);
	if ( thread->Create() == wxTHREAD_NO_ERROR )  thread->Run();
}

