#include "FbTreeView.h"

#include <wx/settings.h>
#include <wx/renderer.h>

const wxChar* wxTreeListCtrlNameStr = _T("treelistctrl");

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

	protected:
		wxTreeListMainWindow *m_owner;
		const wxCursor *m_currentCursor;
		const wxCursor *m_resizeCursor;

	private:
		void OnPaint( wxPaintEvent &event );
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // reduce flicker
		void OnMouse( wxMouseEvent &event );
		void OnSetFocus( wxFocusEvent &event );

	private:
		// common part of all ctors
		void Init();

		void SendListEvent(wxEventType type, wxPoint pos);

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
		wxTreeListCtrl* m_owner;

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

void wxTreeListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
}

void wxTreeListHeaderWindow::OnMouse (wxMouseEvent &event)
{
	event.Skip();
}

void wxTreeListHeaderWindow::OnSetFocus (wxFocusEvent &WXUNUSED(event))
{
    m_owner->SetFocus();
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
    wxScrolledWindow::Create (parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));

    m_owner = parent;

    return true;
}

wxTreeListMainWindow::~wxTreeListMainWindow()
{
}

void wxTreeListMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event))
{
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
    Refresh();
    AdjustMyScrollbars();
*/
}

void wxTreeListMainWindow::OnScroll (wxScrollWinEvent& event)
{
    // FIXME
#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    wxScrolledWindow::OnScroll(event);
#else
    HandleOnScroll( event );
#endif

    if(event.GetOrientation() == wxHORIZONTAL) {
        m_owner->GetHeaderWindow()->Refresh();
        m_owner->GetHeaderWindow()->Update();
    }
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

    CalculateAndSetHeaderHeight();

    return true;
}

void wxTreeListCtrl::CalculateAndSetHeaderHeight()
{
    if (m_header_win) {

        int h = wxRendererNative::Get().GetHeaderButtonHeight(m_header_win);

        // only update if changed
        if (h != m_headerHeight) {
            m_headerHeight = h;
            DoHeaderLayout();
        }
    }
}

void wxTreeListCtrl::DoHeaderLayout()
{
    int w, h;
    GetClientSize(&w, &h);
    if (m_header_win) {
        m_header_win->SetSize (0, 0, w, m_headerHeight);
        m_header_win->Refresh();
    }
    if (m_main_win) {
        m_main_win->SetSize (0, m_headerHeight, w, h - m_headerHeight);
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

