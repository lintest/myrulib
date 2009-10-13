#include "BaseThread.h"
#include "MyRuLibApp.h"
#include "FbConst.h"
#include "FbParams.h"

wxCriticalSection BaseThread::sm_queue;

void BaseThread::DoStart(const int max, const wxString & msg)
{
    m_text = m_info + wxT(" ") + msg;
    m_progress = 0;
    m_max = max;

    wxUpdateUIEvent event( ID_PROGRESS_START );
    event.SetText(m_text);
    event.SetString(m_text);
    event.SetInt(1000);
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoStep(const wxString & msg)
{
    int pos = 0;
    if (m_max) {
        m_progress++;
        pos = m_progress * 1000 / m_max;
    }

    wxUpdateUIEvent event( ID_PROGRESS_UPDATE );
    event.SetText(m_text);
    event.SetString(msg);
    event.SetInt(pos);
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::DoFinish()
{
    wxUpdateUIEvent event( ID_PROGRESS_FINISH );
    wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void BaseThread::UpdateBooksCount()
{
/*
    int count = 0;
    {
        wxCriticalSectionLocker locker(wxGetApp().m_DbSection);
        wxString sql = wxT("SELECT COUNT(id) FROM books");
        DatabaseLayer * database = wxGetApp().GetDatabase();
        PreparedStatement* ps = database->PrepareStatement(sql);
        if (!ps) return;
        DatabaseResultSet* result = ps->ExecuteQuery();
        if (result && result->Next()) count = result->GetResultInt(1);
        database->CloseResultSet(result);
        database->CloseStatement(ps);
    }
    database->SetValue(DB_BOOKS_COUNT, count);
*/
}
