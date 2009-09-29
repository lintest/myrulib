#include "FbTreeListCtrl.h"

BEGIN_EVENT_TABLE(FbTreeListCtrl, wxTreeListCtrl)
    EVT_SIZE(FbTreeListCtrl::OnSizing)
END_EVENT_TABLE()

FbTreeListCtrl::FbTreeListCtrl(wxWindow *parent, wxWindowID id, long style)
    :wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
}

void FbTreeListCtrl::EmptyCols()
{
	for (size_t i = wxTreeListCtrl::GetColumnCount()-1; i>=0; i--) {
		wxTreeListCtrl::RemoveColumn(i);
	}
	m_ColSizes.Empty();
}

void FbTreeListCtrl::AddColumn (const wxString& text, int width, int flag)
{
    wxTreeListCtrl::AddColumn(text, width, flag, -1, true, false);
    m_ColSizes.Add(width);
}

void FbTreeListCtrl::OnSizing(wxSizeEvent& event)
{
	int sum = 0;
	for (size_t i = 0; i<(size_t)m_ColSizes.Count() && i<(size_t)GetColumnCount(); i++) {
        sum += m_ColSizes[i];
	}

	if (!sum) return;

	int w = event.GetSize().GetWidth() - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) - 6;
    int xx = w;
	for (size_t i = 1; i<(size_t)m_ColSizes.Count() && i<(size_t)GetColumnCount(); i++) {
	    int x = w * m_ColSizes[i] / sum;
        SetColumnWidth(i, x);
        xx -= x;
	}
    SetColumnWidth(0, xx);
	event.Skip();
}

