#ifndef __FBFRAMEBASETHREAD_H__
#define __FBFRAMEBASETHREAD_H__

#include <wx/wx.h>
#include "FbThread.h"
#include "FbFrameBase.h"

class FbAggregateFunction : public wxSQLite3AggregateFunction
{
	public:
		virtual void Aggregate(wxSQLite3FunctionContext& ctx);
		virtual void Finalize(wxSQLite3FunctionContext& ctx);
};

class FbFrameBaseThread: public FbThread
{
	public:
		FbFrameBaseThread(FbFrameBase * frame, FbListMode mode)
			:m_frame(frame), m_mode(mode),
			m_FilterFb2(frame->m_FilterFb2),
			m_FilterLib(frame->m_FilterLib),
			m_FilterUsr(frame->m_FilterUsr),
			m_ListOrder(frame->GetOrderSQL())
		{};
	protected:
		virtual wxString GetSQL(const wxString & condition);
		virtual wxString GetOrder();
		virtual void CreateList(wxSQLite3ResultSet &result);
		virtual void CreateTree(wxSQLite3ResultSet &result);
		virtual void InitDatabase(FbCommonDatabase &database);
		void EmptyBooks();
		void FillBooks(wxSQLite3ResultSet &result);
	protected:
		static wxCriticalSection sm_queue;
		FbAggregateFunction m_aggregate;
		wxWindow * m_frame;
		FbListMode m_mode;
		bool m_FilterFb2;
		bool m_FilterLib;
		bool m_FilterUsr;
		wxString m_ListOrder;
};

#endif // __FBFRAMEBASETHREAD_H__
