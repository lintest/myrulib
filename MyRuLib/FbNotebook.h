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

class FbAuiToolbarTabArt : public wxAuiDefaultTabArt
{
	public:
		FbAuiToolbarTabArt(bool flat = false);

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

	protected:
		void DrawPushButton(wxWindow * win, wxDC& dc, const wxRect& rectOrig, int flags, bool flat);

	private:
		bool m_flat;
};

class NbStyleVC71 : public wxAuiDefaultTabArt
{
public:
    NbStyleVC71();
    wxAuiTabArt* Clone();

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                        const wxRect& in_rect, int close_button_state,
                        wxRect* out_tab_rect, wxRect* out_button_rect,
                        int* x_extent);

    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
                            const wxSize& required_bmp_size);
};

class NbStyleFF2 : public wxAuiDefaultTabArt
{
public:
    NbStyleFF2();
    wxAuiTabArt* Clone();
    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
                        const wxRect& in_rect, int close_button_state,
                        wxRect* out_tab_rect, wxRect* out_button_rect,
                        int* x_extent);

    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
                            const wxSize& required_bmp_size);
};

#endif  // __FBNOTEBOOK_H__
