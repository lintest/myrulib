#include "ProgressBar.h"

// Event table
BEGIN_EVENT_TABLE(ProgressBar, wxStatusBar)
   EVT_SIZE(ProgressBar::OnSize)
END_EVENT_TABLE()

ProgressBar::ProgressBar(wxWindow *parent, wxWindowID id, long style, const wxString &name)
    : wxStatusBar(parent, id, style, name)
{
    progress_bar = new wxGauge(this, -1, 100, wxPoint(0, 0), wxDefaultSize, wxGA_HORIZONTAL | wxGA_SMOOTH);
    SetProgress(0);
}

ProgressBar::~ProgressBar()
{
    wxDELETE(progress_bar);
}

void ProgressBar::OnSize(wxSizeEvent &event)
{
   wxRect r;
   GetFieldRect(0, r);
   progress_bar->SetSize(r);
}

void ProgressBar::SetProgress(int progress)
{
   progress_bar->Show(progress > 0);
   progress_bar->SetValue(progress);
}

