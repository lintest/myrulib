#ifndef __FBNOTEBOOK_H__
#define __FBNOTEBOOK_H__

#include <wx/aui/auibook.h>

class FbAuiNotebook : public wxAuiNotebook
{
public:
	FbAuiNotebook() : wxAuiNotebook() {}
private:
	void OnSubmenu(wxCommandEvent & event) { }
	DECLARE_CLASS(FbAuiNotebook)
	DECLARE_EVENT_TABLE()
};

class WXDLLIMPEXP_AUI WxAuiDefaultTabArt : public wxAuiTabArt
{

public:

    WxAuiDefaultTabArt();
    virtual ~WxAuiDefaultTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);
    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    virtual int ShowDropDown(
                         wxWindow* wnd,
                         const wxAuiNotebookPageArray& items,
                         int active_idx);
    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize);

protected:

    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxColour m_baseColour;
    wxPen m_baseColourPen;
    wxPen m_borderPen;
    wxBrush m_baseColourBrush;
    wxColour m_activeColour;
    wxBitmap m_activeCloseBmp;
    wxBitmap m_disabledCloseBmp;
    wxBitmap m_activeLeftBmp;
    wxBitmap m_disabledLeftBmp;
    wxBitmap m_activeRightBmp;
    wxBitmap m_disabledRightBmp;
    wxBitmap m_activeWindowListBmp;
    wxBitmap m_disabledWindowListBmp;

    int m_fixedTabWidth;
    int m_tabCtrlHeight;
    unsigned int m_flags;
};


class WXDLLIMPEXP_AUI WxAuiSimpleTabArt : public wxAuiTabArt
{

public:

    WxAuiSimpleTabArt();
    virtual ~WxAuiSimpleTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiNotebookPageArray& items,
                 int activeIdx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize);

protected:

    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxPen m_normalBkPen;
    wxPen m_selectedBkPen;
    wxBrush m_normalBkBrush;
    wxBrush m_selectedBkBrush;
    wxBrush m_bkBrush;
    wxBitmap m_activeCloseBmp;
    wxBitmap m_disabledCloseBmp;
    wxBitmap m_activeLeftBmp;
    wxBitmap m_disabledLeftBmp;
    wxBitmap m_activeRightBmp;
    wxBitmap m_disabledRightBmp;
    wxBitmap m_activeWindowListBmp;
    wxBitmap m_disabledWindowListBmp;

    int m_fixedTabWidth;
    unsigned int m_flags;
};

class FbBaseTabArt : public WxAuiDefaultTabArt
{
public:
	FbBaseTabArt();

	void DrawBackground(
				 wxDC& dc,
				 wxWindow* wnd,
				 const wxRect& rect);

protected:
	wxPen m_shadowPen;
};

class FbDefaultTabArt : public FbBaseTabArt
{

public:
	FbDefaultTabArt();

	wxAuiTabArt* Clone();

	void DrawTab(wxDC& dc,
				 wxWindow* wnd,
				 const wxAuiNotebookPage& pane,
				 const wxRect& in_rect,
				 int close_button_state,
				 wxRect* out_tab_rect,
				 wxRect* out_button_rect,
				 int* x_extent);

};

class FbCompactTabArt : public WxAuiSimpleTabArt
{
	public:
		FbCompactTabArt();

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

class FbToolbarTabArt : public WxAuiDefaultTabArt
{
	public:
		FbToolbarTabArt(bool flat = false);

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

class FbVstudioTabArt : public FbBaseTabArt
{
public:
	FbVstudioTabArt() {}
	wxAuiTabArt* Clone();

	void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
						const wxRect& in_rect, int close_button_state,
						wxRect* out_tab_rect, wxRect* out_button_rect,
						int* x_extent);

	int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
							const wxSize& required_bmp_size);
};

class FbMozillaTabArt : public FbBaseTabArt
{
public:
	FbMozillaTabArt() {}
	wxAuiTabArt* Clone();

	void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page,
						const wxRect& in_rect, int close_button_state,
						wxRect* out_tab_rect, wxRect* out_button_rect,
						int* x_extent);

	int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages,
							const wxSize& required_bmp_size);
};

#endif  // __FBNOTEBOOK_H__
