#ifndef __VACUUMTHREAD_H__
#define __VACUUMTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>

class VacuumThread : public wxThread
{
	public:
		void Execute();
		VacuumThread (): wxThread() {};
	protected:
		virtual void *Entry();
};

#endif // __VACUUMTHREAD_H__
