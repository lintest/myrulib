#ifndef __TITLETHREAD_H__
#define __TITLETHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbBookThread.h"
#include "FbDatabase.h"

class TitleThread : public FbBookThread
{
public:
	TitleThread (FbBookThread * thread): FbBookThread(thread) {};
	virtual void *Entry();
private:
	wxString GetBookInfo(FbDatabase &database);
	wxString GetBookFiles(FbDatabase &database);
private:
	static wxCriticalSection sm_queue;
};

#endif // __TITLETHREAD_H__
