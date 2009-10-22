#ifndef __FBLOGSTREAM_H__
#define __FBLOGSTREAM_H__

#include <wx/wx.h>
#include <wx/log.h>
#include <wx/thread.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class FbLogStream: public wxLog
{
	public:
		FbLogStream(const wxString & filename);
	protected:
		virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
		virtual void DoLogString(const wxChar *szString, time_t t);
		virtual void PostMsg(wxLogLevel level, const wxChar *szString, time_t t);
	private:
		static wxCriticalSection sm_queue;
		wxFileOutputStream m_stream;
		wxTextOutputStream m_text;
};

#endif // __FBLOGSTREAM_H__
