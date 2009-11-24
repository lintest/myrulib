#ifndef __FBBOOKTHREAD_H__
#define __FBBOOKTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>

class FbBookThread: public wxThread
{
	public:
		FbBookThread(wxEvtHandler *frame, int id, const bool bVertical, const bool bEditable)
			: m_frame(frame), m_id(id), m_vertical(bVertical), m_editable(bEditable) {};
		FbBookThread(FbBookThread * thread)
			: m_frame(thread->m_frame), m_id(thread->m_id), m_vertical(thread->m_vertical), m_editable(thread->m_editable) {};
		void UpdateInfo();
		static wxString HTMLSpecialChars(const wxString &value, const bool bSingleQuotes = false, const bool bDoubleQuotes = true);
		void Execute() { if (Create() == wxTHREAD_NO_ERROR) Run(); };
	protected:
		wxEvtHandler * m_frame;
		int m_id;
		bool m_vertical;
		bool m_editable;
};

#endif // __FBBOOKTHREAD_H__
