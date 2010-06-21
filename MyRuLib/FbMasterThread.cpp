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
	: m_owner(owner), m_info(NULL), m_thread(NULL), m_book(0)
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
	m_book = 0;
}

void FbMasterThread::Open(int book)
{
	wxCriticalSectionLocker locker(sm_section);
	m_book = book;
}

FbMasterInfo * FbMasterThread::GetInfo()
{
	wxCriticalSectionLocker locker(sm_section);
	FbMasterInfo * result = m_info;
	m_info = NULL;
	return result;
}

int FbMasterThread::GetBook()
{
	wxCriticalSectionLocker locker(sm_section);
	int book = m_book;
	m_book = 0;
	return book;
}

void * FbMasterThread::Entry()
{
	while (!TestDestroy()) {
		int book = GetBook();
		if (book) (new ShowThread(m_owner, book, false, false))->Execute();

		FbMasterInfo * info = GetInfo();
		if (info == NULL) continue;

		if (m_thread && m_thread->IsAlive()) m_thread->Pause();
		if (m_thread) m_thread->Wait();
		wxDELETE(m_thread);

		m_thread = new FbJoinedThread(m_owner, info);
		if (m_thread) m_thread->Execute();
		wxDELETE(info);
	}
	return NULL;
}
