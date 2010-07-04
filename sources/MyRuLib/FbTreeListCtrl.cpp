#include "FbTreeListCtrl.h"

BEGIN_EVENT_TABLE(FbTreeListCtrl, wxTreeListCtrl)
	EVT_SIZE(FbTreeListCtrl::OnSizing)
END_EVENT_TABLE()

FbTreeListCtrl::FbTreeListCtrl(wxWindow *parent, wxWindowID id, long style)
	:wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
	SetSortedColumn(0);
}

void FbTreeListCtrl::AddColumn (const wxString& text, int width, int flag)
{
	m_ColSizes.Add(width);
	wxTreeListCtrl::AddColumn(text, width, flag, -1, true, false);
}

void FbTreeListCtrl::OnSizing(wxSizeEvent& event)
{
	DoResizeCols(event.GetSize().GetWidth());
	event.Skip();
}

void FbTreeListCtrl::DoResizeCols(int width)
{
	int w = width - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) - 6;

	int sum = 0;
	for (size_t i = 0; i<(size_t)m_ColSizes.Count() && i<(size_t)GetColumnCount(); i++) {
		if (IsColumnShown(i)) sum += m_ColSizes[i];
	}

	if (!sum) return;

	int xx = w;
	for (size_t i = 1; i<(size_t)m_ColSizes.Count() && i<(size_t)GetColumnCount(); i++) {
		if (IsColumnShown(i)) {
			int x = w * m_ColSizes[i] / sum;
			SetColumnWidth(i, x);
			xx -= x;
		}
	}
	SetColumnWidth(0, xx);
}

void FbTreeListCtrl::Update()
{
	DoResizeCols(GetClientSize().x);
	wxTreeListCtrl::Update();
}

wxTreeItemData * FbTreeListCtrl::GetSelectedData() const
{
    wxTreeItemId selected = GetSelection();
    return selected.IsOk() ? GetItemData(selected) : NULL;
}
