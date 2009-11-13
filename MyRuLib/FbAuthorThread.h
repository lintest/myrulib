#ifndef __FBAUTHORTHREAD_H__
#define __FBAUTHORTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbBookEvent.h"

class FbAuthorThread: public wxThread
{
	public:
		FbAuthorThread(wxControl * frame): m_number(sm_skiper.NewNumber()), m_frame(frame) {};
		void Execute();
	protected:
		virtual void * FbAuthorThread::Entry();
		virtual wxSQLite3ResultSet GetResult(wxSQLite3Database &database) = 0;
	private:
		static wxCriticalSection sm_queue;
		static FbThreadSkiper sm_skiper;
		int m_number;
		wxWindow * m_frame;
};

class FbAuthorThreadChar: public FbAuthorThread
{
	public:
		FbAuthorThreadChar(wxControl * frame, const wxChar & letter)
			:FbAuthorThread(frame), m_letter(letter) {};
	protected:
		virtual wxSQLite3ResultSet GetResult(wxSQLite3Database &database);
	private:
		wxChar m_letter;
};

class FbAuthorThreadText: public FbAuthorThread
{
	public:
		FbAuthorThreadText(wxControl * frame, const wxString & text)
			:FbAuthorThread(frame), m_text(text) {};
	protected:
		virtual wxSQLite3ResultSet GetResult(wxSQLite3Database &database);
	private:
		wxString m_text;
};

class FbAuthorThreadCode: public FbAuthorThread
{
	public:
		FbAuthorThreadCode(wxControl * frame, const int code)
			:FbAuthorThread(frame), m_code(code) {};
	protected:
		virtual wxSQLite3ResultSet GetResult(wxSQLite3Database &database);
	private:
		int m_code;
};

#endif // __FBAUTHORTHREAD_H__
