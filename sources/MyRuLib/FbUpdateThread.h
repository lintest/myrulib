#ifndef __FBUPDATETHREAD_H__
#define __FBUPDATETHREAD_H__

#include <wx/wx.h>
#include "FbThread.h"
#include "FbBookEvent.h"
#include "FbDatabase.h"

class FbUpdateThread: public FbThread
{
	public:
		FbUpdateThread(const wxString &sql1 = wxEmptyString, const wxString &sql2 = wxEmptyString, const wxString &sql3 = wxEmptyString)
			:m_sql1(sql1), m_sql2(sql2), m_sql3(sql3) {};
	protected:
		static wxCriticalSection sm_queue;
		void ExecSQL(FbDatabase &database, const wxString &sql);
		virtual void * Entry();
		wxString m_sql1;
		wxString m_sql2;
		wxString m_sql3;
};

class FbFolderUpdateThread: public FbUpdateThread
{
	public:
		FbFolderUpdateThread(const wxString &sql, const int folder, const FbFolderType type, const wxString &sql2 = wxEmptyString)
			: FbUpdateThread(sql, sql2), m_folder(folder), m_type(type) {};
	protected:
		virtual void * Entry();
		int m_folder;
		FbFolderType m_type;
};

class FbCreateDownloadThread: public FbFolderUpdateThread
{
	public:
		FbCreateDownloadThread(const wxString &sql, const int folder, const FbFolderType type, const wxString &sql2 = wxEmptyString)
			: FbFolderUpdateThread(sql, folder, type, sql2) {};
	protected:
		virtual void * Entry();
};

class FbDeleteThread: public FbUpdateThread
{
	public:
		FbDeleteThread(const wxString &sel): m_sel(sel) {}
	protected:
		virtual void * Entry();
	private:
		void DoDelete(FbDatabase &database);
		const wxString m_sel;
};

class FbGenreThread: public FbThread
{
	public:
		FbGenreThread() {}
	protected:
		virtual void * Entry();
};

#endif // __FBUPDATETHREAD_H__
