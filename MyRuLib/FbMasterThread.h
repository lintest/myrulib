#ifndef __FBMASTERTHREAD_H__
#define __FBMASTERTHREAD_H__

#include <wx/event.h>
#include <wx/thread.h>
#include "FbThread.h"
#include "FbMasterInfo.h"

class FbMasterThread : public FbThread
{
	public:
		FbMasterThread(wxEvtHandler * owner);
		virtual ~FbMasterThread();
		void Reset(const FbMasterInfo &info);
	    virtual void Close();
	protected:
		virtual void * Entry();
	private:
		wxMutex m_mutex;
		wxCondition m_condition;
		wxEvtHandler * m_owner;
		FbMasterInfo m_info;
		FbThread * m_thread;
		bool m_closed;
};

#endif // __FBMASTERTHREAD_H__
