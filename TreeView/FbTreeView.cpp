#include "FbTreeView.h"

#include <wx/settings.h>
#include <wx/renderer.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>
#include "../MyRuLib/FbLogoBitmap.h"


class WXDLLEXPORT wxControlRenderer;

const wxChar* wxTreeListCtrlNameStr = _T("treelistctrl");

class wxTreeListColumnInfo
{
    public:
		wxTreeListColumnInfo() {}

        wxTreeListColumnInfo(
            unsigned int model_column,
            const wxString &text = wxEmptyString,
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
		wxTreeListHeaderWindow( wxWindow *win,
								wxWindowID id,
								wxTreeListMainWindow *owner,
								const wxPoint &pos = wxDefaultPosition,
								const wxSize &size = wxDefaultSize,
								long style = 0,
								const wxString &name = wxT("wxtreelistctrlcolumntitles") );

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
		DECLARE_EVENT_TABLE()
};

class  wxTreeListMainWindow: public wxScrolledWindow
{
	public:
		wxTreeListMainWindow (wxTreeListCtrl *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = wxT("wxtreelistmainwindow"))
		{
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

		unsigned long GetRowCount() { return 500; };

		void SetDirty() { m_dirty = true; }

	private:
		int GetRowHeight(wxDC &dc);
        void AdjustMyScrollbars();

	private:
		wxTreeListCtrl* m_owner;
        wxPen m_dottedPen;
        wxFont m_font;
		int m_current;
        bool m_dirty;

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

BEGIN_EVENT_TABLE(wxTreeListHeaderWindow,wxWindow)
    EVT_PAINT         (wxTreeListHeaderWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxTreeListHeaderWindow::OnEraseBackground) // reduce flicker
    EVT_MOUSE_EVENTS  (wxTreeListHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxTreeListHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

wxTreeListHeaderWindow::wxTreeListHeaderWindow(wxWindow *win, wxWindowID id, wxTreeListMainWindow *owner, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
    : wxWindow(win, id, pos, size, style, name), m_owner(owner), m_sorted(0)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetBackgroundColour(wxSystemSettings::GetColour (wxSYS_COLOUR_BTNFACE));
}

wxTreeListHeaderWindow::~wxTreeListHeaderWindow()
{
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
{
    wxAutoBufferedPaintDC dc( this );
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetFont(GetFont());

    int www, h;
    GetClientSize( &www, &h );
    int w = www - wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    int x = 0;
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( size_t i = 0; i < count && x < www; i++ ) {
        wxHeaderButtonParams params;
        GetColumn(i).Assign(this, params);

        int wCol = GetColumnWidth(i) * w / ww;
        if (i == count - 1) wCol = w - x;
        wxRect rect(x, 0, wCol, h);
        x += wCol;

		wxHeaderSortIconType sort = wxHDR_SORT_ICON_NONE;
		if (abs(m_sorted) == (int)i + 1) m_sorted = i>0 ? wxHDR_SORT_ICON_DOWN : wxHDR_SORT_ICON_UP;

        wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0, sort, &params);
    }

    if (x < www) {
        wxRect rect(x, 0, www-x, h);
        wxRendererNative::Get().DrawHeaderButton(this, dc, rect);
    }
}

int wxTreeListHeaderWindow::XToCol(int x)
{
    int w, left = 0;
    GetClientSize( &w, 0 );
    w -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( size_t col = 0; col < count; col++ ) {
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
	m_current = -1;

    wxScrolledWindow::Create (parent, id, pos, size, style|wxVSCROLL, name);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));

    m_font = wxSystemSettings::GetFont (wxSYS_DEFAULT_GUI_FONT);

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

    m_dirty = true;

    AdjustMyScrollbars();

    return true;
}

wxTreeListMainWindow::~wxTreeListMainWindow()
{
}

int wxTreeListMainWindow::GetRowHeight(wxDC &dc)
{
	dc.SetFont(GetFont());
	int h = (int)(dc.GetCharHeight() + 2);
	return h > FB_CHECKBOX_HEIGHT ? h : FB_CHECKBOX_HEIGHT;
}

void wxTreeListMainWindow::AdjustMyScrollbars()
{
    if (true) {
        wxClientDC dc (this);
        dc.SetFont(GetFont());
        int y = GetRowHeight(dc);

        int xUnit, yUnit;
        GetScrollPixelsPerUnit (&xUnit, &yUnit);
        if (yUnit == 0) yUnit = y;

        int y_pos = GetScrollPos (wxVERTICAL);
        SetScrollbars (0, yUnit, 0, GetRowCount(), 0, y_pos);
    }else{
        SetScrollbars (0, 0, 0, 0);
    }
}

void wxTreeListMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc (this);
    DoPrepareDC (dc);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetFont(GetFont());
    dc.SetBackground(brush);
    dc.Clear();

    int y_pos = GetScrollPos(wxVERTICAL);

    int ww, hh;
    GetClientSize(&ww, &hh);

    int h = GetRowHeight(dc);
    int yy = h * y_pos + hh;

	static wxBitmap bitmaps[3] = {
		wxBitmap(nocheck_xpm),
		wxBitmap(checked_xpm),
		wxBitmap(checkout_xpm),
	};

    for (size_t i = y_pos; i<GetRowCount(); i++)
    {
        int y = i * h;
        if (y>yy) break;

        wxString text = wxString::Format(_("Paint %d"), i);
        for (size_t j=0; j<i%30; j++) text += _(" 0");

		wxRect rect(0, y, ww, h);
        rect.Deflate(2, 2);
		dc.DrawLabel(text, bitmaps[i % 3], rect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }
}

void wxTreeListMainWindow::OnMouse (wxMouseEvent &event)
{
}

void wxTreeListMainWindow::OnChar(wxKeyEvent &event)
{
/*
    // send event to user code
    wxTreeEvent nevent (wxEVT_COMMAND_TREE_KEY_DOWN, 0 );
    nevent.SetInt(m_current);
    nevent.SetKeyEvent (event);
    if (SendEvent(0, NULL, &nevent)) return; // char event handled in user code


    // if no item current, select root
    bool curItemSet = false;
    if (!m_curItem) {
        if (! GetRootItem().IsOk()) return;
        m_curItem = (wxTreeListItem*)GetRootItem().m_pItem;
        if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            m_curItem = (wxTreeListItem*)GetFirstChild (m_curItem, cookie).m_pItem;
        }
        SelectItem(m_curItem, 0L, true);  // unselect others
        curItemSet = true;
    }

    // remember item at shift down
    if (HasFlag(wxTR_MULTIPLE) && event.ShiftDown()) {
        if (!m_shiftItem) m_shiftItem = m_curItem;
    }else{
        m_shiftItem = (wxTreeListItem*)NULL;
    }

    if (curItemSet) return;  // if no item was current until now, do nothing more

    // process all cases
    wxTreeItemId newItem = (wxTreeItemId*)NULL;
    switch (event.GetKeyCode()) {

        // '+': Expand subtree
        case '+':
        case WXK_ADD: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) Expand (m_curItem);
        }break;

        // '-': collapse subtree
        case '-':
        case WXK_SUBTRACT: {
            if (m_curItem->HasPlus() && IsExpanded (m_curItem)) Collapse (m_curItem);
        }break;

        // '*': expand/collapse all subtrees // TODO: Mak it more useful
        case '*':
        case WXK_MULTIPLY: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) {
                ExpandAll (m_curItem);
            }else if (m_curItem->HasPlus()) {
                Collapse (m_curItem); // TODO: CollapseAll
            }
        }break;

        // ' ': toggle current item
        case ' ': {
            SelectItem (m_curItem, (wxTreeListItem*)NULL, false);
        }break;

        // <RETURN>: activate current item
        case WXK_RETURN: {
            if (! SendEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, m_curItem)) {

                // if the user code didn't process the activate event,
                // handle it ourselves by toggling the item when it is
                // double clicked
                if (m_curItem && m_curItem->HasPlus()) Toggle(m_curItem);
            }
        }break;

        // <BKSP>: go to the parent without collapsing
        case WXK_BACK: {
            newItem = GetItemParent (m_curItem);
            if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                newItem = GetPrevSibling (m_curItem); // get sibling instead of root
            }
        }break;

        // <UP>: go to the previous sibling or for the last of its children, to the parent
        case WXK_UP: {
            newItem = GetPrevSibling (m_curItem);
            if (newItem) {
#if !wxCHECK_VERSION(2, 5, 0)
                long cookie = 0;
#else
                wxTreeItemIdValue cookie = 0;
#endif
                while (IsExpanded (newItem) && HasChildren (newItem)) {
                    newItem = GetLastChild (newItem, cookie);
                }
            }else {
                newItem = GetItemParent (m_curItem);
                if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                    newItem = (wxTreeItemId*)NULL; // don't go to root if it is hidden
                }
            }
        }break;

        // <LEFT>: if expanded collapse subtree, else go to the parent
        case WXK_LEFT: {
            if (IsExpanded (m_curItem)) {
                Collapse (m_curItem);
            }else{
                newItem = GetItemParent (m_curItem);
                if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                    newItem = GetPrevSibling (m_curItem); // go to sibling if it is hidden
                }
            }
        }break;

        // <RIGHT>: if possible expand subtree, else go go to the first child
        case WXK_RIGHT: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) {
                Expand (m_curItem);
            }else{
                if (IsExpanded (m_curItem) && HasChildren (m_curItem)) {
#if !wxCHECK_VERSION(2, 5, 0)
                    long cookie = 0;
#else
                    wxTreeItemIdValue cookie = 0;
#endif
                    newItem = GetFirstChild (m_curItem, cookie);
                }
            }
        }break;

        // <DOWN>: if expanded go to the first child, else to the next sibling, ect
        case WXK_DOWN: {
            if (IsExpanded (m_curItem) && HasChildren (m_curItem)) {
#if !wxCHECK_VERSION(2, 5, 0)
                long cookie = 0;
#else
                wxTreeItemIdValue cookie = 0;
#endif
                newItem = GetFirstChild( m_curItem, cookie );
            }
            if (!newItem) {
                wxTreeItemId parent = m_curItem;
                do {
                    newItem = GetNextSibling (parent);
                    parent = GetItemParent (parent);
                } while (!newItem && parent);
            }
        }break;

        // <END>: go to last item of the root
        case WXK_END: {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            newItem = GetLastChild (GetRootItem(), cookie);
			//DENIS KANDRASHIN 2009-10-07 - BEGIN - Change keyboard press END key for last child
            wxTreeItemId child = newItem;
            while (child.IsOk()) {
            	newItem = child;
				wxTreeItemIdValue cookie = 0;
            	child = GetLastChild(newItem, cookie);
            }
            // DENIS - END
        }break;

        // <HOME>: go to root
        case WXK_HOME: {
            newItem = GetRootItem();
            if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
                long cookie = 0;
#else
                wxTreeItemIdValue cookie = 0;
#endif
                newItem = GetFirstChild (newItem, cookie);
            }
        }break;

        // any char: go to the next matching string
        default:
            if (event.GetKeyCode() >= (int)' ') {
                if (!m_findTimer->IsRunning()) m_findStr.Clear();
                m_findStr.Append ((char)event.GetKeyCode());
                m_findTimer->Start (FIND_TIMER_TICKS, wxTIMER_ONE_SHOT);
                wxTreeItemId prev = m_curItem? (wxTreeItemId*)m_curItem: (wxTreeItemId*)NULL;
                while (true) {
                    newItem = FindItem (prev, m_findStr, wxTL_MODE_NAV_EXPANDED |
                                                         wxTL_MODE_FIND_PARTIAL |
                                                         wxTL_MODE_FIND_NOCASE);
                    if (newItem || (m_findStr.Length() <= 1)) break;
                    m_findStr.RemoveLast();
                };
            }
            event.Skip();

    }

    // select and show the new item
    if (newItem) {
        if (!event.ControlDown()) {
            bool unselect_others = !((event.ShiftDown() || event.ControlDown()) &&
                                      HasFlag(wxTR_MULTIPLE));
            SelectItem (newItem, m_shiftItem, unselect_others);
        }
        EnsureVisible (newItem);
        wxTreeListItem *oldItem = m_curItem;
        m_curItem = (wxTreeListItem*)newItem.m_pItem; // make the new item the current item
        RefreshLine (oldItem);
    }
*/
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
    if (!m_dirty) return;
    m_dirty = false;

//    CalculatePositions();
    Refresh();
    AdjustMyScrollbars();
}

void wxTreeListMainWindow::OnScroll (wxScrollWinEvent& event)
{
//    // FIXME
//    #if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
//    wxScrolledWindow::OnScroll(event);
//    #else
//    HandleOnScroll( event );
//    #endif
	HandleOnScroll( event );
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
		m_main_win->SetDirty();
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

void wxTreeListCtrl::AddColumn(unsigned int model_column, const wxString& text, int width, int flag)
{
    if (m_header_win) m_header_win->AddColumn(wxTreeListColumnInfo(model_column, text, width, flag));
}

void wxTreeListCtrl::SetSortedColumn(int column)
{
    if (m_header_win) m_header_win->SetSortedColumn(column);
}

int wxTreeListCtrl::GetSortedColumn()
{
    return m_header_win ? m_header_win->GetSortedColumn() : 0;
}



