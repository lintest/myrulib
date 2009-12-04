#ifndef __FBAUTHORTHREAD_H__
#define __FBAUTHORTHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbBookEvent.h"

class FbAuthorThread: public FbThread
{
	public:
		FbAuthorThread(wxWindow * frame, int order):  m_frame(frame), m_number(sm_skiper.NewNumber()), m_order(order) {};
	protected:
		virtual void * Entry();
		virtual void GetResult(wxSQLite3Database &database) = 0;
		void FillAuthors(wxSQLite3ResultSet &result);
		virtual wxString GetSQL(const wxString & condition);
		wxString GetOrder();
	private:
		static wxCriticalSection sm_queue;
		static FbThreadSkiper sm_skiper;
		wxWindow * m_frame;
		int m_number;
		int m_order;
};

class FbAuthorThreadChar: public FbAuthorThread
{
	public:
		FbAuthorThreadChar(wxWindow * frame, const wxString & letter, int order)
			:FbAuthorThread(frame, order), m_letter(letter) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_letter;
};

class FbAuthorThreadText: public FbAuthorThread
{
	public:
		FbAuthorThreadText(wxWindow * frame, const wxString & mask, int order)
			:FbAuthorThread(frame, order), m_mask(mask) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_mask;
};

class FbAuthorThreadLast: public FbAuthorThread
{
	public:
		FbAuthorThreadLast(wxWindow * frame, const wxString & last)
			:FbAuthorThread(frame, 1), m_last(last) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		wxString m_last;
};

class FbAuthorThreadCode: public FbAuthorThread
{
	public:
		FbAuthorThreadCode(wxWindow * frame, const int code, int order)
			:FbAuthorThread(frame, order), m_code(code) {};
	protected:
		virtual void GetResult(wxSQLite3Database &database);
	private:
		int m_code;
};

#endif // __FBAUTHORTHREAD_H__
