#ifndef __FBAUTHORTHREAD_H__
#define __FBAUTHORTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbBookEvent.h"

class FbAuthorThread: public FbThread
{
	public:
		FbAuthorThread(wxWindow * frame): m_number(sm_skiper.NewNumber()), m_frame(frame) {};
	protected:
		virtual void * Entry();
		virtual void GetResult(wxSQLite3Database &database) = 0;
		void FillAuthors(wxSQLite3ResultSet &result);
		virtual wxString GetSQL(const wxString & condition);
	private:
		static wxCriticalSection sm_queue;
		static FbThreadSkiper sm_skiper;
		int m_number;
		wxWindow * m_frame;
};

class FbAuthorThreadChar: public FbAuthorThread
{
	public:
		FbAuthorThreadChar(wxWindow * frame, const wxChar & letter)
			:FbAuthorThread(frame), m_letter(letter) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxChar m_letter;
};

class FbAuthorThreadText: public FbAuthorThread
{
	public:
		FbAuthorThreadText(wxWindow * frame, const wxString & text)
			:FbAuthorThread(frame), m_text(text) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_text;
};

class FbAuthorThreadCode: public FbAuthorThread
{
	public:
		FbAuthorThreadCode(wxWindow * frame, const int code)
			:FbAuthorThread(frame), m_code(code) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		int m_code;
};

#endif // __FBAUTHORTHREAD_H__
