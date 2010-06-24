#ifndef __FBPREVIEWTHREAD_H__
#define __FBPREVIEWTHREAD_H__

#include <wx/event.h>
#include "FbThread.h"
#include "FbViewItem.h"

class FbPreviewThread : public FbThread
{
	public:
		FbPreviewThread(wxEvtHandler * owner);
		virtual ~FbPreviewThread();
		void Reset(const FbViewItem &view);
	    virtual void Close();
	protected:
		virtual void * Entry();
	private:
		FbViewItem GetView();
		static wxCriticalSection sm_section;
		FbCondition m_condition;
		wxEvtHandler * m_owner;
		FbViewItem m_view;
		FbThread * m_thread;
		bool m_exit;
};

#endif // __FBPREVIEWTHREAD_H__
