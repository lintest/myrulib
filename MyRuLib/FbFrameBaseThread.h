#ifndef __FBFRAMEBASETHREAD_H__
#define __FBFRAMEBASETHREAD_H__

#include <wx/wx.h>
#include "FbFrameBase.h"

class FbFrameBaseThread: public wxThread
{
	public:
		FbFrameBaseThread(FbFrameBase * frame, FbListMode mode)
			:m_frame(frame), m_mode(mode),
			m_FilterFb2(frame->m_FilterFb2),
			m_FilterLib(frame->m_FilterLib),
			m_FilterUsr(frame->m_FilterUsr)
		{};
	protected:
		virtual wxString GetSQL(const wxString & condition, const wxString & order = wxEmptyString);
		virtual void CreateList(wxSQLite3ResultSet &result);
		virtual void CreateTree(wxSQLite3ResultSet &result);
		void EmptyBooks();
		void FillBooks(wxSQLite3ResultSet &result);
	protected:
		static wxCriticalSection sm_queue;
		wxWindow * m_frame;
		FbListMode m_mode;
		bool m_FilterFb2;
		bool m_FilterLib;
		bool m_FilterUsr;
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
