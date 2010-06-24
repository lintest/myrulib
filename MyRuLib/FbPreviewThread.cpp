#include "FbPreviewThread.h"
#include "InfoCash.h"

//-----------------------------------------------------------------------------
//  FbPreviewThread
//-----------------------------------------------------------------------------

wxCriticalSection FbPreviewThread::sm_section;

FbPreviewThread::FbPreviewThread(wxEvtHandler * owner)
	: m_owner(owner), m_thread(NULL), m_exit(false)
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

void FbPreviewThread::Reset(const FbViewItem &view)
{
	wxCriticalSectionLocker locker(sm_section);
	m_view = view;
	m_condition.Signal();
}

FbViewItem FbPreviewThread::GetView()
{
	wxCriticalSectionLocker locker(sm_section);
	FbViewItem result = m_view;
	m_view = FbViewItem::None;
	return result;
}

void * FbPreviewThread::Entry()
{
	while (true) {
		FbViewItem view;
		while (!view) {
			if (IsClosed()) return NULL;
			m_condition.Wait();
			view = GetView();
		}
		if (IsClosed()) return NULL;

		if (m_thread) {
			m_thread->Close();
			m_thread->Wait();
			wxDELETE(m_thread);
		}
//		m_thread = new FbJoinedThread(m_owner, view);
//		if (m_thread) m_thread->Execute();
	}
	return NULL;
}

