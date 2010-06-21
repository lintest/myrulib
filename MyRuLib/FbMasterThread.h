#ifndef __FBMASTERTHREAD_H__
#define __FBMASTERTHREAD_H__

#include <wx/event.h>
#include "FbThread.h"

class FbMasterInfo;

class FbMasterThread : public FbThread
{
	public:
		FbMasterThread(wxEvtHandler * owner);
		virtual ~FbMasterThread();
		void Reset(FbMasterInfo * info);
		void Open(int book);
	protected:
		virtual void * Entry();
	private:
		FbMasterInfo * GetInfo();
		int GetBook();
	private:
		static wxCriticalSection sm_section;
		wxEvtHandler * m_owner;
		FbMasterInfo * m_info;
		FbThread * m_thread;
		int m_book;
};

#endif // __FBMASTERTHREAD_H__
