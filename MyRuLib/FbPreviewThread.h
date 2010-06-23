#ifndef __FBPREVIEWTHREAD_H__
#define __FBPREVIEWTHREAD_H__

#include <wx/event.h>
#include "FbThread.h"
#include "FbMasterInfo.h"

class FbPreviewThread : public FbThread
{
	public:
		FbPreviewThread(wxEvtHandler * owner);
		virtual ~FbPreviewThread();
		void Reset(const FbMasterInfo &info);
	    virtual void Close();
	protected:
		virtual void * Entry();
	private:
		FbMasterInfo GetInfo();
		static wxCriticalSection sm_section;
		FbCondition m_condition;
		wxEvtHandler * m_owner;
		FbMasterInfo m_info;
		FbThread * m_thread;
		bool m_exit;
};

#endif // __FBPREVIEWTHREAD_H__
