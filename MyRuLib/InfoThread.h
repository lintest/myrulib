#ifndef __INFOTHREAD_H__
#define __INFOTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>

class InfoThread : public wxThread
{
public:
    InfoThread(wxEvtHandler *frame, const int id): wxThread(), m_id(id), m_frame(frame) {};
    virtual void *Entry();
    static void Execute(wxEvtHandler *frame, const int id);
private:
	bool Load(wxInputStream& stream);
private:
	static wxCriticalSection sm_queue;
    int m_id;
    wxEvtHandler *m_frame;
};

#endif // __INFOTHREAD_H__
