#ifndef __FBAUTHORTHREAD_H__
#define __FBAUTHORTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbBookEvent.h"

class FbAuthorThread: public FbThread
{
	public:
		FbAuthorThread(wxEvtHandler * frame, int order):  m_frame(frame), m_number(sm_skiper.NewNumber()), m_order(order) {};
	protected:
		virtual void * Entry();
		virtual void GetResult(wxSQLite3Database &database) = 0;
		void FillAuthors(wxSQLite3ResultSet &result);
		virtual wxString GetSQL(const wxString & condition);
		wxString GetOrder();
	private:
		static wxCriticalSection sm_queue;
		static FbThreadSkiper sm_skiper;
		wxEvtHandler * m_frame;
		int m_number;
		int m_order;
};

class FbAuthorThreadChar: public FbAuthorThread
{
	public:
		FbAuthorThreadChar(wxEvtHandler * frame, const wxString & letter, int order)
			:FbAuthorThread(frame, order), m_letter(letter) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_letter;
};

class FbAuthorThreadText: public FbAuthorThread
{
	public:
		FbAuthorThreadText(wxEvtHandler * frame, const wxString & mask, int order)
			:FbAuthorThread(frame, order), m_mask(mask) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_mask;
};

class FbAuthorThreadLast: public FbAuthorThread
{
	public:
		FbAuthorThreadLast(wxEvtHandler * frame, const wxString & last)
			:FbAuthorThread(frame, 1), m_last(last) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_last;
};

class FbAuthorThreadCode: public FbAuthorThread
{
	public:
		FbAuthorThreadCode(wxEvtHandler * frame, const int code, int order)
			:FbAuthorThread(frame, order), m_code(code) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		int m_code;
};

class FbAuthorThreadRepl: public FbAuthorThread
{
	public:
		FbAuthorThreadRepl(wxEvtHandler * frame, const wxString & mask, int id)
			:FbAuthorThread(frame, 1), m_mask(mask), m_id(id) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_mask;
		int m_id;
};

#endif // __FBAUTHORTHREAD_H__
