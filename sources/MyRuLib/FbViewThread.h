#ifndef __FBVIEWTHREAD_H__
#define __FBVIEWTHREAD_H__

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/wxsqlite3.h>
#include "FbCacheBook.h"
#include "controls/FbViewItem.h"
#include "FbThread.h"
#include "FbViewContext.h"

class FbViewData;

class FbViewThread: public FbThread
{
	public:
		FbViewThread(wxEvtHandler * frame, class FbViewContext &ctx, const FbViewItem &view)
			: FbThread(wxTHREAD_JOINABLE), m_frame(frame), m_ctx(ctx), m_view(view) {}
		void SendHTML(const FbViewData &data);
	protected:
		virtual void * Entry();
	private:
		void OpenAuth();
		void OpenBook();
		void OpenNone();
		void SendHTML(wxWindowID winid, const wxString &html);
		wxString GetDescr(wxSQLite3Database &database);
		wxString GetFiles(wxSQLite3Database &database);
	private:
		wxEvtHandler * m_frame;
		FbViewContext m_ctx;
		FbViewItem m_view;
		FbCacheBook m_book;
};

#endif // __FBVIEWTHREAD_H__
