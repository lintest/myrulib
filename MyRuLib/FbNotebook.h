#ifndef __FBNOTEBOOK_H__
#define __FBNOTEBOOK_H__

#include "wx/aui/auibook.h"

class FbAuiDefaultTabArt : public wxAuiDefaultTabArt
{

public:
	FbAuiDefaultTabArt();

    wxAuiTabArt* Clone();

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& in_rect,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);

};


class FbAuiSimpleTabArt : public wxAuiSimpleTabArt
{

public:
	FbAuiSimpleTabArt();

    wxAuiTabArt* Clone();

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& in_rect,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);

protected:
    wxColour m_base_colour;
};

#endif  // __FBNOTEBOOK_H__
