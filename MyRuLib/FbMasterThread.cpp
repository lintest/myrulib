#include "FbMasterThread.h"
#include "FbMasterInfo.h"
#include "FbBookPanel.h"

wxCriticalSection FbMasterThread::sm_section;

FbMasterThread::FbMasterThread(FbBookPanel * owner)
	: m_owner(owner), m_info(NULL), m_thread(NULL), m_modified(false)
{
}

FbMasterThread::~FbMasterThread()
{
	if (m_thread) m_thread->Wait();
	wxDELETE(m_thread);
	wxDELETE(m_info);
}

void FbMasterThread::Reset(FbMasterInfo * info)
{
	wxCriticalSectionLocker locker(sm_section);
	m_info = info;
	m_modified = true;
}

FbMasterInfo * FbMasterThread::GetInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbMasterInfo * result = m_info;
	m_info = NULL;
	return result;
}
bool FbMasterThread::IsModified()
{
	wxCriticalSectionLocker locker(sm_section);
	return m_modified;
}

void * FbMasterThread::Entry()
{
	while (!TestDestroy()) {
		if (!IsModified()) continue;

		if (m_thread) m_thread->Wait();
		wxDELETE(m_thread);

		FbMasterInfo * info = GetInfo();
		if (info) m_thread = info->GetThread(m_owner);
		if (m_thread) m_thread->Execute();
		wxDELETE(info);
	}
	return NULL;
}
