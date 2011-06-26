#include "FbToolBar.h"
#include "FbParams.h"

FbToolBarImages::FbToolBarImages(wxToolBar & toolbar, const wxString &sample)
	: m_toolbar(toolbar)
{
	m_font_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	m_tool_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	m_brush = wxBrush(m_tool_colour, wxSOLID);
	m_font = FbParams(FB_FONT_TOOL);

	wxBitmap bitmap(10, 10);
	wxMemoryDC dc;
	dc.SelectObject(bitmap);
	dc.SetFont(m_font);
	m_size = dc.GetTextExtent(sample);
	m_size.IncBy(2, 0);
	m_toolbar.SetToolBitmapSize(m_size);

	m_rect = m_size;
}

wxBitmap FbToolBarImages::operator[](const wxString &text)
{
	wxBitmap bitmap(m_size.GetX(), m_size.GetY());
	wxMemoryDC dc;
	dc.SelectObject(bitmap);
	dc.SetBackground(m_brush);
	dc.Clear();
	dc.SetFont(m_font);
	dc.SetTextForeground(m_font_colour);
	dc.DrawLabel(text, wxNullBitmap, m_rect, wxALIGN_CENTER);
	dc.SelectObject(wxNullBitmap);
	bitmap.SetMask(new wxMask(bitmap, m_tool_colour));

	return bitmap;
}
