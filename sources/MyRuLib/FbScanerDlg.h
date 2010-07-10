#ifndef __FBSCANERDLG_H__
#define __FBSCANERDLG_H__

#include <wx/timer.h>
#include "FbWindow.h"
#include "FbScanerThread.h"

class FbScanerDlg: public FbDialog
{
	public:
		FbScanerDlg( wxWindow* parent, const wxFileName &filename, const wxFileName &dirname, bool only_new);
		virtual ~FbScanerDlg(void);
	private:
		FbScanerThread m_thread;
		wxStaticText m_text;
		wxGauge m_gauge1;
		wxGauge m_gauge2;
		wxTimer m_timer;
	private:
		void OnProgress1(wxCommandEvent & event);
		void OnProgress2(wxCommandEvent & event);
		void OnTimer(wxTimerEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBSCANERDLG_H__
