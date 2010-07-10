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
		FbScanerThread(wxEvtHandler * owner, const wxFileName &filename, const wxFileName &dirname, bool only_new)
			: FbThread(wxTHREAD_JOINABLE), m_owner(owner), m_filename(filename), m_dirname(dirname), m_only_new(only_new) {}
		FbDatabase & GetDatabase() { return m_database; }
		void Progress1(const wxString & text);
		void Progress2(int position);
	protected:
		virtual void * Entry();
		virtual void OnExit();
		void SavePath();
	private:
		FbMainDatabase m_database;
		wxEvtHandler * m_owner;
		const wxFileName m_filename;
		const wxFileName m_dirname;
		unsigned int m_max;
		unsigned int m_pos;
		bool m_only_new;
};

#endif // __FBSCANERTHREAD_H__
