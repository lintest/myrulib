#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <wx/wx.h>
#include <wx/statusbr.h>

class ProgressBar : public wxStatusBar
{
private:
	wxGauge m_progress;
	void Resize();
public:
	ProgressBar() {}
	ProgressBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString &name = wxT("ProgressBar"));
	bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, long style = wxST_SIZEGRIP, const wxString& name = wxT("ProgressBar"));
	void OnSize(wxSizeEvent &event) { Resize(); };
	void SetProgress(int progress);
	DECLARE_EVENT_TABLE();
};

#endif // PROGRESSBAR_H
