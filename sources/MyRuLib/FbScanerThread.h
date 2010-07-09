#ifndef __FBSCANERTHREAD_H__
#define __FBSCANERTHREAD_H__

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/event.h>
#include <wx/filename.h>
#include "FbThread.h"
#include "FbDatabase.h"

class FbScanerThread: public FbThread
{
	public:
		FbScanerThread(wxEvtHandler * owner, const wxFileName &filename, const wxFileName &dirname)
			: FbThread(wxTHREAD_JOINABLE), m_owner(owner), m_filename(dirname), m_dirname(dirname) {}
		FbDatabase & GetDatabase() { return m_database; }
		void Progress(const wxString & text);
	protected:
		virtual void * Entry();
		void SavePath();
	private:
		FbDatabase m_database;
		wxEvtHandler * m_owner;
		const wxFileName m_filename;
		const wxFileName m_dirname;
};

#endif // __FBSCANERTHREAD_H__