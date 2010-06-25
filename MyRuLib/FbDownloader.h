#ifndef __FBDOWNLOADER_H__
#define __FBDOWNLOADER_H__

#include "FbThread.h"

class FbDownloader: public FbThread
{
	public:
		FbDownloader();
		static wxString GetFilename(const wxString &md5sum, bool bCreateFolder = false);
		void Signal() { m_condition.Signal(); }
	protected:
		virtual void *Entry();
		wxString GetBook();
	private:
		static wxCriticalSection sm_section;
		FbCondition m_condition;
};

#endif // __FBDOWNLOADER_H__
