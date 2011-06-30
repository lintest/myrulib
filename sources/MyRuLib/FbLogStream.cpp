#include "FbLogStream.h"
#include "MyRuLibApp.h"
#include "FbConst.h"

wxCriticalSection FbLog::sm_queue;

wxArrayString FbLog::sm_lines;

void FbLog::PostMsg(wxLogLevel level, const wxChar *szString, time_t t)
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
	wxCriticalSectionLocker enter(sm_queue);
	sm_lines.Add(prefix + szString);
}

bool FbLog::Update(wxArrayString &lines)
{
	wxCriticalSectionLocker enter(sm_queue);
	size_t count = sm_lines.Count();
	for (size_t i = 0; i < count; i++) {
		lines.Add(sm_lines[i]);
	}
	sm_lines.Empty();
	return count;
}


#ifdef FB_SYSLOG_LOGGING

#include "config.h"

extern "C" {
#include <syslog.h>
}

FbLogSyslog::FbLogSyslog()
{
	openlog(PACKAGE_NAME, LOG_PID, LOG_USER);
}

FbLogSyslog::~FbLogSyslog()
{
	closelog();
}

void FbLogSyslog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	switch ( level ) {
		case wxLOG_FatalError: {
			wxString msg = wxString(_("Fatal error")) + COLON + szString;
			syslog(LOG_EMERG, msg.mb_str());
			Flush();
			#ifdef __WXWINCE__
			ExitThread(3);
			#else
			abort();
			#endif
			} break;

		case wxLOG_Error:
			syslog(LOG_ERR, wxString(szString).mb_str());
			PostMsg(level, szString, t);
			break;

		case wxLOG_Warning:
			syslog(LOG_WARNING, wxString(szString).mb_str());
			PostMsg(level, szString, t);
			break;

		case wxLOG_Info:
			syslog(LOG_INFO, wxString(szString).mb_str());
			PostMsg(level, szString, t);
			break;

		case wxLOG_Trace:
		case wxLOG_Debug:
			#ifdef __WXDEBUG__
			syslog(LOG_DEBUG, wxString(szString).mb_str());
			#endif // Debug
			break;

		case wxLOG_Message:
		case wxLOG_Status:
		default:	// log unknown log levels too
			syslog(LOG_NOTICE, wxString(szString).mb_str());
			break;
	}
}

#else // FB_SYSLOG_LOGGING

#include "FbDatabase.h"

static wxString GetLogFile()
{
	wxFileName logname = FbDatabase::GetConfigName();
	logname.SetExt(wxT("log"));
	return logname.GetFullPath();
}

FbLogStream::FbLogStream()
	: m_stream(GetLogFile()), m_text(m_stream)
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
			DoLogString(wxString(wxT("i ")) + szString, t);
			PostMsg(level, szString, t);
			break;

		case wxLOG_Message:
			DoLogString(wxString(wxT("> ")) + szString, t);
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

		case wxLOG_Status:
		default:	// log unknown log levels too
				DoLogString(szString, t);
			break;

	}
}

#endif // FB_SYSLOG_LOGGING
