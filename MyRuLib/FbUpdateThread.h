#ifndef __FBUPDATETHREAD_H__
#define __FBUPDATETHREAD_H__

#include <wx/wx.h>
#include "FbBookEvent.h"
#include "FbDatabase.h"

class FbUpdateThread: public wxThread
{
	public:
		FbUpdateThread(const wxString &sql, const int folder, const FbFolderType type, const wxString &sql2 = wxEmptyString)
			:m_sql(sql), m_folder(folder), m_type(type), m_sql2(sql2) {};
	protected:
		static wxCriticalSection sm_queue;
		void ExecSQL(FbDatabase &database, const wxString &sql);
		virtual void * Entry();
		wxString m_sql;
		int m_folder;
		FbFolderType m_type;
		wxString m_sql2;
};

class FbCreateDownloadThread: public FbUpdateThread
{
	public:
		FbCreateDownloadThread(const wxString &sql, const int folder, const FbFolderType type, const wxString &sql2 = wxEmptyString)
			:FbUpdateThread(sql, folder, type, sql2) {};
	protected:
		virtual void * Entry();
};

#endif // __FBUPDATETHREAD_H__
