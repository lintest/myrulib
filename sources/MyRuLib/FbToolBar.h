#ifndef __FBTOOLBAR_H__
#define __FBTOOLBAR_H__

#include <wx/wx.h>

class FbToolBarImages
{
	public:
		FbToolBarImages(wxToolBar & toolbar, const wxString &sample);
		wxBitmap operator[](const wxString &text);
	private:
		wxToolBar & m_toolbar;
	private:
		wxColour m_font_colour;
		wxColour m_tool_colour;
		wxBrush m_brush;
		wxFont m_font;
		wxSize m_size;
		wxRect m_rect;

};

#endif // __FBTOOLBAR_H__
