#ifndef __REGTHREAD_H__
#define __REGTHREAD_H__

#include <wx/wx.h>

class RegThread : public wxThread
{
public:
    RegThread(wxEvtHandler *frame, const wxString &filename);
    virtual void *Entry();
private:
    int AddArchive(int min, int max);
private:
    unsigned m_count;
    wxString m_filename;
    wxEvtHandler *m_frame;
};

#endif // __REGTHREAD_H__
