#include "FbTreeView.h"

#include <wx/settings.h>
#include <wx/renderer.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>

class WXDLLEXPORT wxControlRenderer;

const wxChar* wxTreeListCtrlNameStr = _T("treelistctrl");

class wxTreeListColumnInfo
{
    public:
        wxTreeListColumnInfo() {};

        wxTreeListColumnInfo(
            const wxString &text = wxEmptyString,
            unsigned int model_column,
            int width = DEFAULT_COL_WIDTH,
            int flag = wxALIGN_LEFT
        ) : m_text(text), m_model_column(model_column), m_width(width), m_flag(flag) {};

        wxTreeListColumnInfo(const wxTreeListColumnInfo &info)
            : m_text(info.m_text), m_model_column(info.m_model_column), m_width(info.m_width), m_flag(info.m_flag) {};

        void Assign(wxTreeListHeaderWindow * header, wxHeaderButtonParams &params) const;

        int GetWidth() const { return m_width; };

    private:
        wxString m_text;
        unsigned int m_model_column;
        int m_width;
        int m_flag;

    private:
        friend class wxTreeListMainWindow;
};

static wxTreeListColumnInfo wxInvalidTreeListColumnInfo;

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(wxTreeListColumnInfo, wxArrayTreeListColumn);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayTreeListColumn);

class  wxTreeListHeaderWindow : public wxWindow
{
	public:
		wxTreeListHeaderWindow();

		wxTreeListHeaderWindow( wxWindow *win,
								wxWindowID id,
								wxTreeListMainWindow *owner,
								const wxPoint &pos = wxDefaultPosition,
								const wxSize &size = wxDefaultSize,
								long style = 0,
								const wxString &name = _T("wxtreelistctrlcolumntitles") );

		virtual ~wxTreeListHeaderWindow();

        void AddColumn(const wxTreeListColumnInfo & info) { m_columns.Add(info); };

        size_t GetColumnCount() const { return m_columns.Count(); };

        const wxTreeListColumnInfo & GetColumn (size_t column) const {
            wxCHECK_MSG (column < GetColumnCount(), wxInvalidTreeListColumnInfo, _T("Invalid column"));
            return m_columns[column];
        }

        int GetColumnWidth(size_t column) { return GetColumn(column).GetWidth(); };

        int GetFullWidth();

        int XToCol(int x);

        int GetSortedColumn() { return m_sorted; }

        void SetSortedColumn(int column) { m_sorted = column; }

	private:
		wxTreeListMainWindow * m_owner;
		const wxCursor * m_currentCursor;
		const wxCursor * m_resizeCursor;
		wxArrayTreeListColumn m_columns;
		int m_sorted;

	private:
		void Init();
        void SendListEvent(wxEventType type, wxPoint pos, int colunm);

	private:
		void OnPaint( wxPaintEvent &event );
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // reduce flicker
		void OnMouse( wxMouseEvent &event );
		void OnSetFocus( wxFocusEvent &event );
		DECLARE_DYNAMIC_CLASS(wxTreeListHeaderWindow)
		DECLARE_EVENT_TABLE()
};

class  wxTreeListMainWindow: public wxScrolledWindow
{
	public:
		// creation
		// --------
		wxTreeListMainWindow() { Init(); }

		wxTreeListMainWindow (wxTreeListCtrl *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = _T("wxtreelistmainwindow"))
		{
			Init();
			Create (parent, id, pos, size, style, validator, name);
		}

		virtual ~wxTreeListMainWindow();

		bool Create(wxTreeListCtrl *parent, wxWindowID id = -1,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxTR_DEFAULT_STYLE,
					const wxValidator &validator = wxDefaultValidator,
					const wxString& name = _T("wxtreelistctrl"));

		virtual bool SetBackgroundColour (const wxColour& colour);
		virtual bool SetForegroundColour (const wxColour& colour);

	protected:
        void AdjustMyScrollbars();
		wxTreeListCtrl* m_owner;
        wxPen m_dottedPen;

	private:
		void OnPaint( wxPaintEvent &event );
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // to reduce flicker
		void OnSetFocus( wxFocusEvent &event );
		void OnKillFocus( wxFocusEvent &event );
		void OnChar( wxKeyEvent &event );
		void OnMouse( wxMouseEvent &event );
		void OnIdle( wxIdleEvent &event );
		void OnScroll(wxScrollWinEvent& event);
		void OnCaptureLost(wxMouseCaptureLostEvent &event) { ;; }

	private:
		DECLARE_EVENT_TABLE()
		DECLARE_DYNAMIC_CLASS(wxTreeListMainWindow)
};

//-----------------------------------------------------------------------------
//  wxTreeListHeaderWindow
//-----------------------------------------------------------------------------

void wxTreeListColumnInfo::Assign(wxTreeListHeaderWindow * header, wxHeaderButtonParams &params) const
{
    params.m_labelFont = header->GetFont();
    params.m_labelText = m_text;
    params.m_labelAlignment = wxALIGN_LEFT;
    params.m_labelColour = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
}

//-----------------------------------------------------------------------------
//  wxTreeListHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListHeaderWindow,wxWindow);

BEGIN_EVENT_TABLE(wxTreeListHeaderWindow,wxWindow)
    EVT_PAINT         (wxTreeListHeaderWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxTreeListHeaderWindow::OnEraseBackground) // reduce flicker
    EVT_MOUSE_EVENTS  (wxTreeListHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxTreeListHeaderWindow::OnSetFocus)
END_EVENT_TABLE()


void wxTreeListHeaderWindow::Init()
{
    m_currentCursor = (wxCursor *) NULL;
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    m_sorted = 0;
}

wxTreeListHeaderWindow::wxTreeListHeaderWindow()
{
    Init();

    m_owner = (wxTreeListMainWindow *) NULL;
    m_resizeCursor = (wxCursor *) NULL;
}

wxTreeListHeaderWindow::wxTreeListHeaderWindow( wxWindow *win,
                                                wxWindowID id,
                                                wxTreeListMainWindow *owner,
                                                const wxPoint& pos,
                                                const wxSize& size,
                                                long style,
                                                const wxString &name )
    : wxWindow( win, id, pos, size, style, name )
{
    Init();

    m_owner = owner;
    m_resizeCursor = new wxCursor(wxCURSOR_SIZEWE);

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_BTNFACE));
}

wxTreeListHeaderWindow::~wxTreeListHeaderWindow()
{
    wxDELETE(m_resizeCursor);
}

int wxTreeListHeaderWindow::GetFullWidth()
{
    int ww = 0;
    size_t count = m_columns.Count();
    for ( size_t i = 0; i < count; i++ ) ( ww += m_columns[i].GetWidth() );
    if (!ww) ww = 1;
    return ww;
}

void wxTreeListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
//void wxTreeListHeaderWindow::DoDraw(wxControlRenderer *renderer)
{
    wxAutoBufferedPaintDC dc( this );
    dc.SetBackgroundMode(wxTRANSPARENT);

    int w, h;
    GetClientSize( &w, &h );

    int x = 0;
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( int i = 0; i < count && x < w; i++ )
    {
        wxHeaderButtonParams params;
        GetColumn(i).Assign(this, params);

        int wCol = GetColumnWidth(i) * w / ww;
        if (i == count-1) wCol = w - x;
        wxRect rect(x, 0, wCol, h);
        x += wCol;

		wxHeaderSortIconType sort = wxHDR_SORT_ICON_NONE;
		if (i+1 == GetSortedColumn()) sort = wxHDR_SORT_ICON_DOWN;
		if (i+1 == - GetSortedColumn()) sort = wxHDR_SORT_ICON_UP;

        wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0, sort, &params);
    }

    if (x < w) {
        wxRect rect(x, 0, w-x, h);
        wxRendererNative::Get().DrawHeaderButton(this, dc, rect);
    }
}

int wxTreeListHeaderWindow::XToCol(int x)
{
    int w, left = 0;
    GetClientSize( &w, 0 );
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( int col = 0; col < count; col++ ) {
        left += GetColumnWidth(col) * w / ww;
        if (x < left) return col;
    }
    return count - 1;
}

void wxTreeListHeaderWindow::OnMouse (wxMouseEvent &event)
{
    if (event.LeftUp() && m_sorted) {
        int col = XToCol(event.GetX());
        if (abs(m_sorted) == col + 1) {
            m_sorted = - m_sorted;
        } else {
            m_sorted = col + 1;
        }
        SendListEvent (wxEVT_COMMAND_LIST_COL_CLICK, event.GetPosition(), col);
        Refresh();
    }
	event.Skip();
}

void wxTreeListHeaderWindow::OnSetFocus (wxFocusEvent &WXUNUSED(event))
{
    m_owner->SetFocus();
}

void wxTreeListHeaderWindow::SendListEvent (wxEventType type, wxPoint pos, int colunm)
{
    wxWindow *parent = GetParent();
    wxListEvent le (type, parent->GetId());
    le.SetEventObject (parent);
    le.m_pointDrag = pos;

    // the position should be relative to the parent window, not
    // this one for compatibility with MSW and common sense: the
    // user code doesn't know anything at all about this header
    // window, so why should it get positions relative to it?
    le.m_pointDrag.y -= GetSize().y;
    le.m_col = colunm;
    parent->GetEventHandler()->ProcessEvent (le);
}

// ---------------------------------------------------------------------------
// wxTreeListMainWindow implementation
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListMainWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxTreeListMainWindow, wxScrolledWindow)
    EVT_PAINT          (wxTreeListMainWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxTreeListMainWindow::OnEraseBackground) // to reduce flicker
    EVT_MOUSE_EVENTS   (wxTreeListMainWindow::OnMouse)
    EVT_CHAR           (wxTreeListMainWindow::OnChar)
    EVT_SET_FOCUS      (wxTreeListMainWindow::OnSetFocus)
    EVT_KILL_FOCUS     (wxTreeListMainWindow::OnKillFocus)
    EVT_IDLE           (wxTreeListMainWindow::OnIdle)
    EVT_SCROLLWIN      (wxTreeListMainWindow::OnScroll)
    EVT_MOUSE_CAPTURE_LOST(wxTreeListMainWindow::OnCaptureLost)
END_EVENT_TABLE()

// ---------------------------------------------------------------------------
// construction/destruction
// ---------------------------------------------------------------------------

bool wxTreeListMainWindow::Create (wxTreeListCtrl *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator &validator,
                                   const wxString& name)
{
    wxScrolledWindow::Create (parent, id, pos, size, style|wxVSCROLL, name);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));

#ifdef __WXMSW__
    {
        int i, j;
        wxBitmap bmp(8, 8);
        wxMemoryDC bdc;
        bdc.SelectObject(bmp);
        bdc.SetPen(*wxGREY_PEN);
        bdc.DrawRectangle(-1, -1, 10, 10);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if (!((i + j) & 1)) {
                    bdc.DrawPoint(i, j);
                }
            }
        }

        m_dottedPen = wxPen(bmp, 1);
    }
#else
//?    m_dottedPen = wxPen( *wxGREY_PEN, 1, wxDOT );  // too slow under XFree86
    m_dottedPen = wxPen( _T("grey"), 0, 0 ); // Bitmap based pen is not supported by GTK!
#endif

    m_owner = parent;

    AdjustMyScrollbars();

    return true;
}

wxTreeListMainWindow::~wxTreeListMainWindow()
{
}

void wxTreeListMainWindow::AdjustMyScrollbars()
{
    if (true) {
        wxClientDC dc (this);
        int y = (int)(dc.GetCharHeight() + 2);

        int xUnit, yUnit;
        GetScrollPixelsPerUnit (&xUnit, &yUnit);
        if (yUnit == 0) yUnit = y;

        int y_pos = GetScrollPos (wxVERTICAL);
        SetScrollbars (0, yUnit, 0, 30, 0, y_pos);
    }else{
        SetScrollbars (0, 0, 0, 0);
    }
}

void wxTreeListMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc (this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
    DoPrepareDC (dc);
}

void wxTreeListMainWindow::OnMouse (wxMouseEvent &event)
{
}

void wxTreeListMainWindow::OnChar (wxKeyEvent &event)
{
}

void wxTreeListMainWindow::OnSetFocus (wxFocusEvent &event)
{
/*
    m_hasFocus = true;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
    event.Skip();
}

void wxTreeListMainWindow::OnKillFocus( wxFocusEvent &event )
{
/*
    m_hasFocus = false;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
    event.Skip();
}

void wxTreeListMainWindow::OnIdle (wxIdleEvent &WXUNUSED(event))
{
/*
    if (!m_dirty) return;

    m_dirty = false;

    CalculatePositions();
*/
    Refresh();
    AdjustMyScrollbars();
}

void wxTreeListMainWindow::OnScroll (wxScrollWinEvent& event)
{
    // FIXME
#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    wxScrolledWindow::OnScroll(event);
#else
    HandleOnScroll( event );
#endif
}

bool wxTreeListMainWindow::SetBackgroundColour (const wxColour& colour)
{
    if (!wxWindow::SetBackgroundColour(colour)) return false;

    Refresh();
    return true;
}

bool wxTreeListMainWindow::SetForegroundColour (const wxColour& colour)
{
    if (!wxWindow::SetForegroundColour(colour)) return false;

    Refresh();
    return true;
}

//-----------------------------------------------------------------------------
//  wxTreeListCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListCtrl, wxControl);

BEGIN_EVENT_TABLE(wxTreeListCtrl, wxControl)
    EVT_SIZE(wxTreeListCtrl::OnSize)
END_EVENT_TABLE();

bool wxTreeListCtrl::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style, const wxValidator &validator,
                            const wxString& name)
{
    long main_style = style & ~(wxSIMPLE_BORDER|wxSUNKEN_BORDER|wxDOUBLE_BORDER|wxRAISED_BORDER|wxSTATIC_BORDER);

	main_style |= wxWANTS_CHARS ;

    long ctrl_style = style & ~(wxVSCROLL|wxHSCROLL);

    if (!wxControl::Create(parent, id, pos, size, ctrl_style, validator, name)) {
       return false;
    }

    m_main_win = new wxTreeListMainWindow (this, -1, wxPoint(0, 0), size, main_style, validator);

    m_header_win = new wxTreeListHeaderWindow (this, -1, m_main_win, wxPoint(0, 0), wxDefaultSize, wxTAB_TRAVERSAL);

    DoHeaderLayout();

    return true;
}

void wxTreeListCtrl::DoHeaderLayout()
{
    int x, y;
    GetClientSize(&x, &y);
    int h = 0;
    if (m_header_win) {
        h = wxRendererNative::Get().GetHeaderButtonHeight(m_header_win);
        #ifdef __WXMSW__
        h = h * 4 / 5 + 2;
        #endif
        m_header_win->SetSize (0, 0, x, h);
        m_header_win->Refresh();
    }
    if (m_main_win) {
        m_main_win->SetSize (0, h, x, y - h);
    }
}

void wxTreeListCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoHeaderLayout();
}

bool wxTreeListCtrl::SetBackgroundColour(const wxColour& colour)
{
    if (!m_main_win) return false;
    return m_main_win->SetBackgroundColour(colour);
}

bool wxTreeListCtrl::SetForegroundColour(const wxColour& colour)
{
    if (!m_main_win) return false;
    return m_main_win->SetForegroundColour(colour);
}

wxSize wxTreeListCtrl::DoGetBestSize() const
{
    return wxSize (200,200);
}

void wxTreeListCtrl::SetFocus()
{
	m_main_win->SetFocus();
}

void wxTreeListCtrl::Refresh(bool erase, const wxRect* rect)
{
    m_main_win->Refresh (erase, rect);
    m_header_win->Refresh (erase, rect);
}

bool wxTreeListCtrl::SetFont(const wxFont& font)
{
    if (m_header_win) {
        m_header_win->SetFont(font);
        DoHeaderLayout();
        m_header_win->Refresh();
    }
    if (m_main_win) {
        return m_main_win->SetFont(font);
    }else{
        return false;
    }
}

void wxTreeListCtrl::AddColumn(const wxString& text, unsigned int model_column, int width, int flag)
{
    if (m_header_win) m_header_win->AddColumn(wxTreeListColumnInfo(text, model_column, width, flag));
}

void wxTreeListCtrl::SetSortedColumn(int column)
{
    if (m_header_win) m_header_win->SetSortedColumn(column);
}

int wxTreeListCtrl::GetSortedColumn()
{
    return m_header_win ? m_header_win->GetSortedColumn() : 0;
}

