#include "FbPreviewThread.h"
#include "InfoCash.h"

//-----------------------------------------------------------------------------
//  FbPreviewThread
//-----------------------------------------------------------------------------

wxCriticalSection FbPreviewThread::sm_section;

FbPreviewThread::FbPreviewThread(wxEvtHandler * owner)
	: m_owner(owner), m_info(NULL), m_thread(NULL), m_exit(false)
{
}

FbPreviewThread::~FbPreviewThread()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
	}
	wxDELETE(m_thread);
}

void FbPreviewThread::Close()
{
	FbThread::Close();
	m_condition.Signal();
}

void FbPreviewThread::Reset(const FbMasterInfo &info)
{
	wxCriticalSectionLocker locker(sm_section);
	m_info = info;
	m_condition.Signal();
}

FbMasterInfo FbPreviewThread::GetInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbMasterInfo result = m_info;
	m_info = NULL;
	return result;
}

void * FbPreviewThread::Entry()
{
	while (true) {
		FbMasterInfo info = GetInfo();
		while (!info) {
			if (IsClosed()) return NULL;
			m_condition.Wait();
			info = GetInfo();
		}
		if (IsClosed()) return NULL;

		if (m_thread) {
			m_thread->Close();
			m_thread->Wait();
			wxDELETE(m_thread);
		}
//		m_thread = new FbJoinedThread(m_owner, info);
//		if (m_thread) m_thread->Execute();
	}
	return NULL;
}

