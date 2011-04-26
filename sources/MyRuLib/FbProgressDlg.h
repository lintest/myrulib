#ifndef __FBSCANERDLG_H__
#define __FBSCANERDLG_H__

#include <wx/timer.h>
#include "FbWindow.h"
#include "FbThread.h"
#include "FbBookEvent.h"

class FbProgressDlg: public FbDialog
{
	public:
		FbProgressDlg(wxWindow* parent);
		virtual ~FbProgressDlg(void);
		void RunThread(FbThread * thread);
	private:
		FbThread * m_thread;
		wxStaticText m_text;
		wxGauge m_gauge;
		wxTimer m_timer;
	private:
		void OnTimer(wxTimerEvent& event);
		void OnGaugeStart(FbProgressEvent & event);
		void OnGaugeUpdate(FbProgressEvent & event);
		void OnGaugePulse(FbProgressEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBSCANERDLG_H__
