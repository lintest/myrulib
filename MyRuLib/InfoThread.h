#ifndef __INFOTHREAD_H__
#define __INFOTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbBookThread.h"
#include "ParseCtx.h"

class InfoThread:
	public ParsingContext,
	public FbBookThread
{
	public:
		InfoThread(wxEvtHandler *frame, const int id, bool m_vertical)
			: FbBookThread(frame, id, m_vertical) {};
		InfoThread(FbBookThread * thread)
			: FbBookThread(thread) {};
		virtual void *Entry();
		static void Execute(wxEvtHandler *frame, const int id, const bool vertical);
		int GetId() { return m_id; };
	public:
		wxString annotation;
		wxString imagedata;
		wxString imagetype;
		wxString imagename;
		bool skipimage;
		wxArrayString images;
	private:
		bool Load(wxInputStream& stream);
	private:
		static wxCriticalSection sm_queue;
};

#endif // __INFOTHREAD_H__
