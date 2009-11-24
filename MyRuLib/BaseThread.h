#ifndef __BASETHREAD_H__
#define __BASETHREAD_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbThread.h"
#include "polarssl/md5.h"

class BaseThread : public wxThread
{
	public:
		static wxString CalcMd5(md5_context &md5);
	public:
		wxString m_info;
		wxString m_text;
	protected:
		void DoStart(const int max, const wxString & msg);
		void DoStep(const wxString & msg);
		void DoFinish();
	protected:
		static wxCriticalSection sm_queue;
	private:
		int m_progress;
		int m_max;
};

#endif // __BASETHREAD_H__
