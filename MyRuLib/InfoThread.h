#ifndef __INFOTHREAD_H__
#define __INFOTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>

class InfoThread : public wxThread
{
public:
    InfoThread(wxEvtHandler *frame, const int id, const bool vertical)
		: wxThread(), m_frame(frame), m_id(id), m_vertical(vertical) {};
    virtual void *Entry();
    static void Execute(wxEvtHandler *frame, const int id, const bool vertical);
private:
	bool Load(wxInputStream& stream);
private:
	static wxCriticalSection sm_queue;
    wxEvtHandler *m_frame;
    int m_id;
    bool m_vertical;
};

#endif // __INFOTHREAD_H__
