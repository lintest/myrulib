#ifndef __FBUPDATETHREAD_H__
#define __FBUPDATETHREAD_H__

#include <wx/wx.h>
#include "FbThread.h"
#include "FbDatabase.h"
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
		FbUpdateItem(FbDatabase & database, int code, const wxString &type);
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
		FbDatabase & m_database;
		int m_code;
		wxString m_type;
		wxString m_url;
		wxString m_filename;
		wxString m_dataname;
		DECLARE_CLASS(FbUpdateItem);
};

class FbFulltextThread: public FbProgressThread
{
	public:
		FbFulltextThread(wxEvtHandler * owner)
			: FbProgressThread(owner, wxTHREAD_JOINABLE) {}
	protected:
		virtual void * Entry();
};

#endif // __FBUPDATETHREAD_H__
