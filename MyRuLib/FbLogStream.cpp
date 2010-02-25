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
	m_stream.SeekO(0, wxFromEnd);

	wxDateTime datetime(t);
	wxString text = datetime.FormatISODate() + wxT(" ");
	text += datetime.FormatISOTime() + wxT(" ");
	text += szString;
	text += wxT("\n");

	wxCriticalSectionLocker enter(sm_queue);
	m_text.WriteString(text);
}

void FbLogStream::PostMsg(wxLogLevel level, const wxChar *szString, time_t t)
{
	wxString prefix;
	switch ( level ) {
		case wxLOG_Error:
			prefix = wxT("E> ");
			break;
		case wxLOG_Warning:
			prefix = wxT("!> ");
			break;
		case wxLOG_Info:
			prefix = wxT("i> ");
			break;
		default:
			prefix = wxEmptyString;
			break;
	}

	wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_ERROR );
	event.SetInt(level);
	event.SetTimestamp(t);
	event.SetString(prefix + szString);
	wxPostEvent(wxGetApp().GetTopWindow(), event);
}

void FbLogStream::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	switch ( level ) {
		case wxLOG_FatalError:
			DoLogString(wxString(_("Fatal error")) + COLON + szString, t);
			DoLogString(_("Program aborted."), t);
			Flush();
			#ifdef __WXWINCE__
			ExitThread(3);
			#else
			abort();
			#endif
			break;

		case wxLOG_Error:
			DoLogString(wxString(wxT("E ")) + szString, t);
			PostMsg(level, szString, t);
			break;

		case wxLOG_Warning:
			DoLogString(wxString(wxT("! ")) + szString, t);
			PostMsg(level, szString, t);
			break;

		case wxLOG_Info:
			DoLogString(wxString(wxT("> ")) + szString, t);
			break;

		case wxLOG_Message:
		case wxLOG_Status:
		default:	// log unknown log levels too
				DoLogString(szString, t);
			break;

		case wxLOG_Trace:
		case wxLOG_Debug:
			#ifdef __WXDEBUG__
			{
				wxString msg = level == wxLOG_Trace ? wxT("Trace: ") : wxT("Debug: ");
				msg << szString;
				DoLogString(msg, t);
			}
			#endif // Debug
			break;
	}
}

