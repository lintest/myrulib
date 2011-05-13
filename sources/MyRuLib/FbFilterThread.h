#ifndef __FBFILTERTHREAD_H__
#define __FBFILTERTHREAD_H__

#include "FbThread.h"
#include "FbFilterObj.h"

class FbFilterThread : public FbThread
{
public:
	FbFilterThread(wxEvtHandler * owner, const FbFilterObj & filter)
		: FbThread(wxTHREAD_JOINABLE), m_owner(owner), m_filter(filter) {}

protected:
	virtual void * Entry();

private:
	bool Execute();

private:
	wxEvtHandler * m_owner;
	FbFilterObj m_filter;
	wxString m_tempfile;
};

#endif // __FBFILTERTHREAD_H__
