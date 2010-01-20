#ifndef __EXPTHREAD_H__
#define __EXPTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/arrimpl.cpp>
#include <wx/filename.h>
#include "BaseThread.h"

class ExportFileItem
{
	public:
		ExportFileItem(const wxFileName &n, const int i): filename(n), id(i) {};
	public:
		wxFileName filename;
		int id;
};

WX_DECLARE_OBJARRAY(ExportFileItem, ExportFileArray);

class ExportThread : public BaseThread
{
	public:
		ExportThread(bool compress): BaseThread(), m_compress(compress) {};
		virtual void *Entry();
	public:
		ExportFileArray m_filelist;
		bool Execute();
	private:
		void WriteFileItem(ExportFileItem &item);
	private:
		bool m_compress;
};

#endif // __EXPTHREAD_H__
