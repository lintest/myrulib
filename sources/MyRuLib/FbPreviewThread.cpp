#include "FbPreviewThread.h"
#include "FbViewThread.h"

//-----------------------------------------------------------------------------
//  FbPreviewThread
//-----------------------------------------------------------------------------

FbPreviewThread::FbPreviewThread(wxEvtHandler * owner)
	: FbThread(wxTHREAD_JOINABLE), m_condition(m_mutex), m_owner(owner), m_thread(NULL), m_closed(false)
{
}

FbPreviewThread::~FbPreviewThread()
{
	if (m_thread) {
		m_thread->Close();
		m_thread->Wait();
		wxDELETE(m_thread);
	}
}

void FbPreviewThread::Close()
{
	wxCriticalSectionLocker locker(m_section);
	m_closed = true;
	m_condition.Broadcast();
}

void FbPreviewThread::Reset(const FbViewContext &ctx, const FbViewItem &view)
{
	wxCriticalSectionLocker locker(m_section);
	m_view = view;
	m_ctx = ctx;
	m_condition.Broadcast();
}

void * FbPreviewThread::Entry()
{
	while (true) {
		{
			wxMutexLocker locker(m_mutex);
			m_condition.Wait();
		}

		FbViewContext ctx;
		FbViewItem view;
		{
			wxCriticalSectionLocker lock(m_section);
			if (m_closed) return NULL;
			if (!m_view) continue;
			view = m_view;
			ctx = m_ctx;
			m_view = FbViewItem::None;
		}


		if (m_thread) {
			m_thread->Close();
			m_thread->Wait();
			wxDELETE(m_thread);
		}

		m_thread = new FbViewThread(m_owner, ctx, view);
		if (m_thread) m_thread->Execute();
	}
	return NULL;
}

