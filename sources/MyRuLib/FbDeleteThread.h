#ifndef __FBDELETETHREAD_H__
#define __FBDELETETHREAD_H__

#include <wx/wx.h>
#include "FbThread.h"
#include "FbDatabase.h"

class FbDeleteThread: public FbThread
{
	public:
		FbDeleteThread(const wxString &sel): m_sel(sel) {}
	protected:
		virtual void * Entry();
	private:
		void DoDelete(FbDatabase &database);
		const wxString m_sel;
};

#endif // __FBDELETETHREAD_H__
