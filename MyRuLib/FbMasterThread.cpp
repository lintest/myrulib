#include "FbMasterThread.h"
#include "FbMasterInfo.h"
#include "InfoCash.h"

//-----------------------------------------------------------------------------
//  FbJoinedThread
//-----------------------------------------------------------------------------

class FbJoinedThread: public FbThread
{
	public:
		FbJoinedThread(wxEvtHandler * frame, const FbMasterInfo * info)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_info(info->Clone()) {}
	protected:
		virtual void * Entry()
			{ return m_info->Execute(m_frame, this); }
	private:
		wxEvtHandler * m_frame;
		FbMasterInfo * m_info;
};

//-----------------------------------------------------------------------------
//  FbMasterThread
//-----------------------------------------------------------------------------

wxCriticalSection FbMasterThread::sm_section;

FbMasterThread::FbMasterThread(wxEvtHandler * owner)
	: m_owner(owner), m_info(NULL), m_thread(NULL), m_exit(false)
{
}

FbMasterThread::~FbMasterThread()
{
	if (m_thread) m_thread->Wait();
	wxDELETE(m_thread);
	wxDELETE(m_info);
}

void FbMasterThread::Exit()
{
	wxCriticalSectionLocker locker(sm_section);
	m_exit = true;
	m_condition.Signal();
}

bool FbMasterThread::IsExit()
{
	wxCriticalSectionLocker locker(sm_section);
	return m_exit;
}

void FbMasterThread::Reset(FbMasterInfo * info)
{
	wxCriticalSectionLocker locker(sm_section);
	m_info = info;
	m_condition.Signal();
}

FbMasterInfo * FbMasterThread::GetInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbMasterInfo * result = m_info;
	m_info = NULL;
	return result;
}

void * FbMasterThread::Entry()
{
	while (true) {
		FbMasterInfo * info = GetInfo();
		while (info == NULL) {
			if (TestDestroy() || IsExit()) break;
			m_condition.Wait();
			info = GetInfo();
		}
		if (TestDestroy() || IsExit()) break;

		if (m_thread) m_thread->Wait();
		wxDELETE(m_thread);

		m_thread = new FbJoinedThread(m_owner, info);
		if (m_thread) m_thread->Execute();
		wxDELETE(info);
	}
	return NULL;
}

