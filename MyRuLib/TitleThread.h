#ifndef __TITLETHREAD_H__
#define __TITLETHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbBookThread.h"
#include "FbDatabase.h"

class TitleThread : public FbBookThread
{
public:
    TitleThread(wxEvtHandler *frame, const int id, bool m_vertical)
		: FbBookThread(frame, id, m_vertical) {};
	TitleThread (FbBookThread * thread)
		: FbBookThread(thread) {};
    virtual void *Entry();
    static void Execute(wxEvtHandler *frame, const int id, bool m_vertical);
private:
	wxString GetBookInfo(FbDatabase &database, int id);
	wxString GetBookFiles(FbDatabase &database, int id);
    static wxString HTMLSpecialChars( const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);
private:
	static wxCriticalSection sm_queue;
};

#endif // __TITLETHREAD_H__
