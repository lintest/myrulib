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
		InfoThread(FbBookThread * thread): FbBookThread(thread), parsebody(false) {};
		virtual void *Entry();
		int GetId() { return m_id; };
		void StartTag(wxString &name, const XML_Char **atts);
		void FinishTag(wxString &name);
		void AppendImg(const XML_Char **atts);
		void StartImg(const XML_Char **atts);
		void WriteImg(const XML_Char *s, int len);
		void FinishImg();
		void WriteText(const XML_Char *s, int len);
		void CheckLength();
	public:
		wxString isbn;
		wxString annotation;
		wxString imagedata;
		wxString imagetype;
		wxString imagename;
		bool skipimage;
		bool parsebody;
		wxArrayString images;
	private:
		bool Load(wxInputStream& stream);
	private:
		static wxCriticalSection sm_queue;
};

#endif // __INFOTHREAD_H__
