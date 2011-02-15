#ifndef __FBPREVIEWTHREAD_H__
#define __FBPREVIEWTHREAD_H__

#include <wx/event.h>
#include "FbThread.h"
#include "controls/FbViewItem.h"
#include "FbViewContext.h"

class FbPreviewThread : public FbThread
{
	public:
		FbPreviewThread(wxEvtHandler * owner);
		virtual ~FbPreviewThread();
		void Reset(const FbViewContext &ctx, const FbViewItem &view);
	    virtual void Close();
	protected:
		virtual void * Entry();
	private:
		wxMutex m_mutex;
		wxCondition m_condition;
		wxCriticalSection m_section;
		wxEvtHandler * m_owner;
		FbThread * m_thread;
		FbViewItem m_view;
		FbViewContext m_ctx;
		bool m_closed;
};

#endif // __FBPREVIEWTHREAD_H__
