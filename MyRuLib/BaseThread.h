#ifndef __BASETHREAD_H__
#define __BASETHREAD_H__

#include <wx/wx.h>

class BaseThread : public wxThread
{
    public:
        wxString m_info;
    protected:
        void DoStart(const int max, const wxString & msg);
        void DoError(const wxString & msg);
        void DoStep(const wxString & msg);
        void DoFinish();
    protected:
        static wxCriticalSection sm_queue;
    private:
        int m_progress;
        int m_max;
};

#endif // __BASETHREAD_H__
