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
		InfoThread(FbBookThread * thread): FbBookThread(thread) {};
		virtual void *Entry();
		int GetId() { return m_id; };
	public:
		wxString isbn;
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
