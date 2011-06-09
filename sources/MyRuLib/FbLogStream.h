#ifndef __FBLOGSTREAM_H__
#define __FBLOGSTREAM_H__

#include <wx/wx.h>
#include <wx/log.h>
#include <wx/thread.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class FbLog: public wxLog
{
	public:
		static bool Update(wxArrayString &lines);
	protected:
		void PostMsg(wxLogLevel level, const wxChar *szString, time_t t);
	protected:
		static wxCriticalSection sm_queue;
		static wxArrayString sm_lines;
};

#ifdef FB_SYSLOG_LOGGING

class FbLogSyslog: public FbLog
{
	public:
		FbLogSyslog();
		virtual ~FbLogSyslog();
	protected:
		virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
};

#else // FB_SYSLOG_LOGGING

class FbLogStream: public FbLog
{
	public:
		FbLogStream();
	protected:
		virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
		virtual void DoLogString(const wxChar *szString, time_t t);
	private:
		wxFileOutputStream m_stream;
		wxTextOutputStream m_text;
};

#endif // FB_SYSLOG_LOGGING

#endif // __FBLOGSTREAM_H__
