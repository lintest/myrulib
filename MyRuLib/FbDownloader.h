#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include <wx/wx.h>

class FbDownloadThread: public wxThread
{
	public:
		static void FbDownloadThread::Execute();
		FbDownloadThread(const int id): m_id(id) {};
	protected:
        static wxCriticalSection sm_queue;
		virtual void *Entry();
	private:
		int m_id;
};

#endif // __FBDOWNLOADER_H__
