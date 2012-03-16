#ifndef __FBTHREAD_H__
#define __FBTHREAD_H__

#include <wx/wx.h>

class FbCondition
	: public wxCondition
{
public:
	FbCondition(): wxCondition(m_mutex) {}
private:
	wxMutex m_mutex;
};

class FbThread
	: public wxThread
{
public:
    typedef FbThread * PFbThread;

	static void DeleteRef(PFbThread & thread);

	FbThread(wxThreadKind kind = wxTHREAD_DETACHED)
		: wxThread(kind), m_closed(false) {}

	void Execute()
		{ if ( Create() == wxTHREAD_NO_ERROR ) Run(); }

	bool IsClosed();

	virtual void Close();

private:
	static wxCriticalSection sm_section;
	bool m_closed;
};

class FbProgressThread
	: public FbThread
{
public:
	FbProgressThread(wxEvtHandler * owner, wxThreadKind kind = wxTHREAD_DETACHED)
		: FbThread(kind), m_owner(owner) {}
	wxEvtHandler * GetOwner() { return m_owner; }
public:
	void SetInfo(const wxString & info) { m_info = info; }
	void DoStart(const wxString & msg, int max = 0);
	void DoPulse(const wxString & msg);
	void DoStep(const wxString & msg);
	void DoFinish();
private:
	wxEvtHandler * m_owner;
	wxString m_info;
	int m_pos;
	int m_max;
};

#endif // __FBTHREAD_H__
