#ifndef __FBVIEWTHREAD_H__
#define __FBVIEWTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbViewItem.h"
#include "FbThread.h"
#include "FbViewContext.h"

class FbViewThread: public FbThread
{
	public:
		FbViewThread(wxEvtHandler * frame, class FbViewContext &ctx, const FbViewItem &view)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_ctx(ctx), m_view(view) {}
	protected:
		virtual void * Entry();
	private:
		void OpenAuth();
		void OpenBook();
		void OpenNone();
		void SendHTML(wxWindowID winid, const wxString &html);
		wxString GetDescr();
	private:
		wxEvtHandler * m_frame;
		FbViewContext m_ctx;
		FbViewItem m_view;
};

class FbBookThreadBase: public wxThread
{
	public:
		FbBookThreadBase(wxEvtHandler *frame, int id, const bool bVertical, const bool bEditable)
			: m_frame(frame), m_id(id), m_vertical(bVertical), m_editable(bEditable) {};
		FbBookThreadBase(FbBookThreadBase * thread)
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

#endif // __FBVIEWTHREAD_H__
