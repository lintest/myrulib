#ifndef __FBMASTERTHREAD_H__
#define __FBMASTERTHREAD_H__

#include <wx/event.h>
#include "FbThread.h"

class FbMasterInfo;

class FbCondition: public wxCondition
{
	public:
		FbCondition(): wxCondition(m_mutex) {}
	private:
		wxMutex m_mutex;
};

class FbMasterThread : public FbThread
{
	public:
		FbMasterThread(wxEvtHandler * owner);
		virtual ~FbMasterThread();
		void Reset(FbMasterInfo * info);
		void Open(int book);
		void Exit();
	protected:
		virtual void * Entry();
	private:
		FbMasterInfo * GetInfo();
		int GetBook();
	private:
		static wxCriticalSection sm_section;
		FbCondition m_condition;
		wxEvtHandler * m_owner;
		FbMasterInfo * m_info;
		FbThread * m_thread;
		bool m_exit;
};

#endif // __FBMASTERTHREAD_H__
