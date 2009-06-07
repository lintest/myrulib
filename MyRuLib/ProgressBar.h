#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <wx/wx.h>
#include <wx/statusbr.h>

class ProgressBar : public wxStatusBar
{
private:
   wxGauge *progress_bar;

public:
   ProgressBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString &name = wxT("ProgressBar"));
   ~ProgressBar();

   void OnSize(wxSizeEvent &event);

   void SetProgress(int progress);

   DECLARE_EVENT_TABLE();
};

#endif // PROGRESSBAR_H
