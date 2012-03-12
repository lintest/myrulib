#ifndef __FBFRAMETHREAD_H__
#define __FBFRAMETHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "FbDatabase.h"
#include "FbFilterObj.h"

class FbFrameDatabase : public FbDatabase
{
public:
	FbFrameDatabase(FbThread * thread, wxString &filename);
};

class FbFrameThread : public FbThread
{
public:
	static int GetCount(wxSQLite3Database &database, int code);

	FbFrameThread(wxEvtHandler * frame, const wxString &counter)
		: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_counter(counter) {}

	virtual ~FbFrameThread(void)
		{}

	void SetCountSQL(const wxString &sql, const FbFilterObj &filter);

protected:
	static wxString GetOrder(int order, const wxString &standart);
	void CreateCounter(wxSQLite3Database &database, const wxString &sql);
	virtual void MakeModel(wxSQLite3ResultSet &result) {}

protected:
	wxEvtHandler * m_frame;
	wxString m_counter;
	wxString m_sql;
};

class FbCountThread : public FbFrameThread
{
public:
	FbCountThread(wxEvtHandler * frame) : FbFrameThread(frame, wxEmptyString) {}
protected:
	virtual void * Entry();
};

#endif // __FBFRAMETHREAD_H__
