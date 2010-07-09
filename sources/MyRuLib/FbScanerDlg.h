#ifndef __FBSCANERDLG_H__
#define __FBSCANERDLG_H__

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
	private:
		void OnProgress1(wxCommandEvent & event);
		void OnProgress2(wxCommandEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBSCANERDLG_H__
