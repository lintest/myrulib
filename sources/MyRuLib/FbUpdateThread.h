#ifndef __FBUPDATETHREAD_H__
#define __FBUPDATETHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "controls/FbURL.h"

class FbUpdateThread: public FbThread
{
	public:
		FbUpdateThread();
	protected:
		virtual void * Entry();
};

class FbUpdateItem: public wxObject
{
	public:
		static wxString GetAddr(int date, const wxString &type);
		FbUpdateItem(wxSQLite3Database & database, int code, const wxString &type);
		virtual ~FbUpdateItem();
		int Execute();
	private:
		bool ReadURL();
		bool OpenZip();
		int DoUpdate();
		void ExecInsert();
		void ExecDelete();
		void CalcCount();
	private:
		wxSQLite3Database & m_database;
		int m_code;
		wxString m_type;
		wxString m_url;
		wxString m_filename;
		wxString m_dataname;
		DECLARE_CLASS(FbUpdateItem);
};

#endif // __FBUPDATETHREAD_H__
