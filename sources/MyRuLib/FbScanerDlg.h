#ifndef __FBSCANERDLG_H__
#define __FBSCANERDLG_H__

#include "FbWindow.h"
#include "FbScanerThread.h"

class FbScanerDlg: public FbDialog
{
	public:
		FbScanerDlg(const wxFileName &filename, const wxFileName &dirname);
		virtual ~FbScanerDlg(void);
	private:
		FbScanerThread m_thread;
		wxStaticText m_text;
		wxGauge m_gauge;
	private:
		void OnProgress(wxCommandEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBSCANERDLG_H__