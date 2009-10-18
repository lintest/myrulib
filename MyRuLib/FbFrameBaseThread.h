#ifndef __FBFRAMEBASETHREAD_H__
#define __FBFRAMEBASETHREAD_H__

#include <wx/wx.h>
#include "BooksPanel.h"

class FbFrameBaseThread: public wxThread
{
	public:
        FbFrameBaseThread(wxWindow * frame, FbListMode mode)
			:m_frame(frame), m_mode(mode) {};
		wxString GetSQL(const wxString & condition);
	protected:
		void EmptyBooks();
		void FillBooks(wxSQLite3ResultSet &result);
		static wxCriticalSection sm_queue;
        wxWindow * m_frame;
	private:
		void CreateList(wxSQLite3ResultSet &result);
		void CreateTree(wxSQLite3ResultSet &result);
	private:
        FbListMode m_mode;
};

class FbThreadSkiper
{
	public:
		FbThreadSkiper(): m_number(0) {};
		bool Skipped(const int number) { return number != m_number; };
		int NewNumber() { return ++m_number; };
	private:
		wxCriticalSection m_queue;
		int m_number;
};

#endif // __FBFRAMEBASETHREAD_H__
