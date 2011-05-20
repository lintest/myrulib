#ifndef __FBFRAMETHREAD_H__
#define __FBFRAMETHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"

class FbFrameThread : public FbThread
{
public:
	static void AttachCounter(wxSQLite3Database &database, const wxString &filename);
	static int GetCount(wxSQLite3Database &database, int code);

	FbFrameThread(wxEvtHandler * frame, const wxString &counter)
		: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_counter(counter) {}
	virtual ~FbFrameThread(void)
		{}

protected:
	static wxString GetOrder(int order, const wxString &standart);
	void CreateCounter(wxSQLite3Database &database, const wxString &sql);
	virtual void MakeModel(wxSQLite3ResultSet &result) = 0;

protected:
	wxEvtHandler * m_frame;
	wxString m_counter;
};

#endif // __FBFRAMETHREAD_H__
