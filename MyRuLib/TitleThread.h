#ifndef __TITLETHREAD_H__
#define __TITLETHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbDatabase.h"

class TitleThread : public wxThread
{
public:
    TitleThread(wxEvtHandler *frame, const int id, bool m_vertical)
		: wxThread(), m_frame(frame), m_id(id), m_vertical(m_vertical) {};
    virtual void *Entry();
    static void Execute(wxEvtHandler *frame, const int id, bool m_vertical);
private:
	wxString GetBookInfo(FbDatabase &database, int id);
	wxString GetBookFiles(FbDatabase &database, int id);
    static wxString HTMLSpecialChars( const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);
private:
	static wxCriticalSection sm_queue;
    wxEvtHandler *m_frame;
    int m_id;
    bool m_vertical;
};



#endif // __TITLETHREAD_H__
