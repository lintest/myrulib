#include "FbMasterThread.h"

//-----------------------------------------------------------------------------
//  FbBooksThread
//-----------------------------------------------------------------------------

class FbBooksThread: public FbThread
{
	public:
		FbBooksThread(wxEvtHandler * frame, const FbMasterInfo info, const FbFilterObj &filter)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_info(info), m_filter(filter) {}
	protected:
		virtual void * Entry()
			{ return m_info.Execute(m_frame, this, m_filter); }
	private:
		wxEvtHandler * m_frame;
		FbMasterInfo m_info;
		FbFilterObj m_filter;
};

//-----------------------------------------------------------------------------
//  FbMasterThread
//-----------------------------------------------------------------------------

FbMasterThread::FbMasterThread(wxEvtHandler * owner)
	: FbThread(wxTHREAD_JOINABLE), m_condition(m_mutex), m_owner(owner), m_thread(NULL), m_closed(false)
{
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
	wxCriticalSectionLocker locker(m_section);
	m_closed = true;
	m_condition.Broadcast();
}

void FbMasterThread::Reset(const FbMasterInfo &info, const FbFilterObj &filter)
{
	wxCriticalSectionLocker locker(m_section);
	m_info = info;
	m_filter = filter;
	m_condition.Broadcast();
}

void * FbMasterThread::Entry()
{
	while (true) {
		{
			wxMutexLocker locker(m_mutex);
			m_condition.Wait();
		}

		FbMasterInfo info;
		FbFilterObj filter;
		{
			wxCriticalSectionLocker lock(m_section);
			if (m_closed) return NULL;
			if (!m_info) continue;
			info = m_info;
			filter = m_filter;
			m_info = FbMasterInfo();
		}

		if (m_thread) {
			m_thread->Close();
			m_thread->Wait();
			wxDELETE(m_thread);
		}

		(m_thread = new FbBooksThread(m_owner, info, filter))->Execute();
	}
	return NULL;
}

