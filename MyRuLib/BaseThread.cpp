#include "BaseThread.h"
#include "MyRuLibApp.h"
#include "FbConst.h"

wxCriticalSection BaseThread::sm_queue;

void BaseThread::DoStart(const int max, const wxString & msg)
{
    m_progress = 0;
    m_max = max;
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_START );
    event.SetString(m_info + wxT(" ") + msg);
    event.SetInt(max);
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoError(const wxString & msg)
{
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_ERROR );
    event.SetString(msg);
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoStep(const wxString & msg)
{
    if (m_max) m_progress++;
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_UPDATE );
    event.SetString(msg);
    event.SetInt(m_progress);
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoFinish()
{
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PROGRESS_FINISH );
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}
