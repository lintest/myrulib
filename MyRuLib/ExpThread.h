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
		ExportThread(int format): BaseThread(), m_format(format) {};
		virtual void *Entry();
	public:
		ExportFileArray m_filelist;
		bool Execute();
	private:
		wxString GetScript();
		void WriteFileItem(ExportFileItem &item);
		wxString GetCommand(const wxString &script, const wxFileName &filename);
	private:
		int m_format;
		wxArrayString m_scripts;
};

#endif // __EXPTHREAD_H__
