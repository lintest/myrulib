#ifndef __FBMASTERTHREAD_H__
#define __FBMASTERTHREAD_H__

#include <wx/event.h>
#include <wx/thread.h>
#include "FbThread.h"
#include "FbMasterInfo.h"
#include "FbFilterObj.h"

class FbMasterThread : public FbThread
{
	public:
		FbMasterThread(wxEvtHandler * owner);
		virtual ~FbMasterThread();
		void Reset(const FbMasterInfo &info, const FbFilterObj &filter);
		virtual void Close();
	protected:
		virtual void * Entry();
	private:
		wxMutex m_mutex;
		wxCondition m_condition;
		wxCriticalSection m_section;
		wxEvtHandler * m_owner;
		FbMasterInfo m_info;
		FbFilterObj m_filter;
		FbThread * m_thread;
		bool m_closed;
};

#endif // __FBMASTERTHREAD_H__
