#ifndef __FBBOOKTHREAD_H__
#define __FBBOOKTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>

class FbBookThread: public wxThread
{
	public:
		FbBookThread(wxEvtHandler *frame, int id, bool vertical)
			: m_frame(frame), m_id(id), m_vertical(vertical) {};
		FbBookThread(FbBookThread * thread)
			: m_frame(thread->m_frame), m_id(thread->m_id), m_vertical(thread->m_vertical) {};
		void UpdateInfo();
		static wxString HTMLSpecialChars(const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);
	protected:
		wxEvtHandler * m_frame;
		int m_id;
		bool m_vertical;
};

#endif // __FBBOOKTHREAD_H__
