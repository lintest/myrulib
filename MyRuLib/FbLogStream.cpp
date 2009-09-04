#include "FbLogStream.h"
#include "MyRuLibApp.h"
#include "FbConst.h"

wxCriticalSection FbLogStream::sm_queue;

FbLogStream::FbLogStream(const wxString & filename)
    : m_stream(filename), m_text(m_stream)
{
}

void FbLogStream::DoLogString(const wxChar *szString, time_t t)
{
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_ERROR );
    event.SetString(szString);
    wxPostEvent(wxGetApp().GetTopWindow(), event);

    wxCriticalSectionLocker enter(sm_queue);
    wxDateTime datetime(t);
    wxString text = datetime.FormatISODate() + wxT(" ");
    text += datetime.FormatISOTime() + wxT(" ");
    text += szString;
    text += wxT("\n");
    m_text.WriteString(text);
}

