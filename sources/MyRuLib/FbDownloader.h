#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include "FbThread.h"

class FbDownloader: public wxThread
{
	public:
		FbDownloader();
		static wxString GetFilename(const wxString &md5sum, bool bCreateFolder = false);
		void Signal();
		void Close();
		bool IsClosed();
		void Execute();
	protected:
		virtual void *Entry();
		wxString GetBook();
	private:
		wxMutex m_mutex;
		wxCondition m_condition;
		wxCriticalSection m_section;
		bool m_closed;
};

#endif // __FBDOWNLOADER_H__
