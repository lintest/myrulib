#ifndef __FBMASTERTHREAD_H__
#define __FBMASTERTHREAD_H__

#include "FbThread.h"

class FbBookPanel;

class FbMasterInfo;

class FbMasterThread : public FbThread
{
	public:
		FbMasterThread(FbBookPanel * owner);
		virtual ~FbMasterThread();
		void Reset(FbMasterInfo * info);
	protected:
		virtual void * Entry();
	private:
		FbMasterInfo * GetInfo();
	private:
		static wxCriticalSection sm_section;
		FbBookPanel * m_owner;
		FbMasterInfo * m_info;
		FbThread * m_thread;
};

#endif // __FBMASTERTHREAD_H__
