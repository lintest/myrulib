#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/thread.h>
#include <wx/object.h>

class FbCondition: public wxCondition
{
	public:
		FbCondition(): wxCondition(m_mutex) {}
	private:
		wxMutex m_mutex;
};

class FbThread: public wxThread
{
	public:
	    FbThread(wxThreadKind kind = wxTHREAD_DETACHED)
			: wxThread(kind), m_closed(false) {}
		void Execute()
			{ if ( Create() == wxTHREAD_NO_ERROR ) Run(); }
	    bool IsClosed();
	    virtual void Close();
	private:
		static wxCriticalSection sm_section;
		bool m_closed;
};

#endif // __FBTHREAD_H__
