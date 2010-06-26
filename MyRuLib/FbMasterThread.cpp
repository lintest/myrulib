#include "FbMasterThread.h"

//-----------------------------------------------------------------------------
//  FbJoinedThread
//-----------------------------------------------------------------------------

class FbJoinedThread: public FbThread
{
	public:
		FbJoinedThread(wxEvtHandler * frame, const FbMasterInfo info)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_info(info) {}
	protected:
		virtual void * Entry()
			{ return m_info.Execute(m_frame, this); }
	private:
		wxEvtHandler * m_frame;
		FbMasterInfo m_info;
};

//-----------------------------------------------------------------------------
//  FbMasterThread
//-----------------------------------------------------------------------------

FbMasterThread::FbMasterThread(wxEvtHandler * owner)
	: m_condition(m_mutex), m_owner(owner), m_info(NULL), m_thread(NULL), m_closed(false)
{
	m_mutex.Lock();
}

FbMasterThread::~FbMasterThread()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbMasterThread::Close()
{
	{
		wxCriticalSectionLocker lock(m_section);
		m_closed = true;
	}
	m_condition.Broadcast();
}

void FbMasterThread::Reset(const FbMasterInfo &info)
{
	wxCriticalSectionLocker lock(m_section);
	m_info = info;
	m_condition.Broadcast();
}

void * FbMasterThread::Entry()
{
	while (true) {
		m_condition.Wait();
		wxCriticalSectionLocker lock(m_section);

		if (m_closed) return NULL;
		if (!m_info) continue;

		if (m_thread) {
			m_thread->Close();
			m_thread->Wait();
			wxDELETE(m_thread);
		}
		m_thread = new FbJoinedThread(m_owner, m_info);
		if (m_thread) m_thread->Execute();
	}
	return NULL;
}

