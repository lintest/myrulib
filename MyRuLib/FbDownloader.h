#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include <wx/wx.h>
#include <wx/url.h>

class FbDownloadThread: public wxThread
{
	public:
		static void FbDownloadThread::Execute();
		FbDownloadThread();
//		FbDownloadThread(const int id, wxInputStream * in): m_id(id), m_in(in) {};
	protected:
        static wxCriticalSection sm_queue;
		virtual void *Entry();
	private:
		int m_id;
		wxURL url;
		wxInputStream * m_in;
};

#endif // __FBDOWNLOADER_H__
