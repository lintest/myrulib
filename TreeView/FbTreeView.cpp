#include "FbTreeView.h"

#include <wx/settings.h>
#include <wx/renderer.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>

#include "../MyRuLib/FbLogoBitmap.h"

class FbTreeViewItem;

class WXDLLEXPORT wxControlRenderer;

const wxChar* FbTreeViewCtrlNameStr = _T("treelistctrl");

class FbTreeViewColumnInfo
{
    public:
		FbTreeViewColumnInfo() {}

        FbTreeViewColumnInfo(
            size_t model_column,
            const wxString &text = wxEmptyString,
            int width = DEFAULT_COL_WIDTH,
            int flag = wxALIGN_LEFT
        ) : m_text(text), m_model_column(model_column), m_width(width), m_flag(flag) {};

        FbTreeViewColumnInfo(const FbTreeViewColumnInfo &info)
            : m_text(info.m_text), m_model_column(info.m_model_column), m_width(info.m_width), m_flag(info.m_flag) {};

        void Assign(FbTreeViewHeaderWindow * header, wxHeaderButtonParams &params) const;

        int GetWidth() const { return m_width; };

    private:
        wxString m_text;
        size_t m_model_column;
        int m_width;
        int m_flag;

    private:
        friend class FbTreeViewMainWindow;
};

static FbTreeViewColumnInfo wxInvalidTreeListColumnInfo;

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbTreeViewColumnInfo, wxArrayTreeListColumn);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayTreeListColumn);

class  FbTreeViewHeaderWindow : public wxWindow
{
	public:
		FbTreeViewHeaderWindow( wxWindow *win,
								wxWindowID id,
								FbTreeViewMainWindow *owner,
								const wxPoint &pos = wxDefaultPosition,
								const wxSize &size = wxDefaultSize,
								long style = 0,
								const wxString &name = wxT("FbTreeViewctrlcolumntitles") );

		virtual ~FbTreeViewHeaderWindow();

        void AddColumn(const FbTreeViewColumnInfo & info) { m_columns.Add(info); };

        size_t GetColumnCount() const { return m_columns.Count(); };

        const FbTreeViewColumnInfo & GetColumn (size_t column) const {
            wxCHECK_MSG (column < GetColumnCount(), wxInvalidTreeListColumnInfo, _T("Invalid column"));
            return m_columns[column];
        }

        int GetColumnWidth(size_t column) { return GetColumn(column).GetWidth(); };

        int GetFullWidth();

        int XToCol(int x);

        int GetSortedColumn() { return m_sorted; }

        void SetSortedColumn(int column) { m_sorted = column; }

	private:
		FbTreeViewMainWindow * m_owner;
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

class  FbTreeViewMainWindow: public wxScrolledWindow
{
	public:
		FbTreeViewMainWindow (FbTreeViewCtrl *parent, wxWindowID id = -1,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = wxT("FbTreeViewmainwindow"));

		virtual ~FbTreeViewMainWindow();

		virtual bool SetBackgroundColour (const wxColour& colour);
		virtual bool SetForegroundColour (const wxColour& colour);

		virtual void SetFocus();
        virtual bool SetFont(const wxFont& font);

	public:
		unsigned long GetRowCount() { return m_model ? m_model->GetRowCount() : 0; };
		void SetDirty() { m_dirty = true; }
		void AssignModel(FbTreeModel * model);

	private:
		int GetRowHeight() { return m_rowHeight; };
		int GetClientCount();
        void AdjustMyScrollbars();
		bool SendEvent(wxEventType type, FbTreeViewItem *item = NULL, wxTreeEvent *event = NULL);  // returns true if processed

	private:
		FbTreeViewCtrl* m_owner;
        FbTreeModel * m_model;
        wxPen m_dottedPen;
		int m_current;
        bool m_dirty;
        int m_rowHeight;

	private:
		void OnPaint( wxPaintEvent &event );
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // to reduce flicker
        void OnLeftDown( wxMouseEvent& event );
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
//  FbTreeViewHeaderWindow
//-----------------------------------------------------------------------------

void FbTreeViewColumnInfo::Assign(FbTreeViewHeaderWindow * header, wxHeaderButtonParams &params) const
{
    params.m_labelFont = header->GetFont();
    params.m_labelText = m_text;
    params.m_labelAlignment = wxALIGN_LEFT;
    params.m_labelColour = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
}

//-----------------------------------------------------------------------------
//  FbTreeViewHeaderWindow
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbTreeViewHeaderWindow,wxWindow)
    EVT_ERASE_BACKGROUND(FbTreeViewHeaderWindow::OnEraseBackground)
    EVT_PAINT(FbTreeViewHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (FbTreeViewHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (FbTreeViewHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

FbTreeViewHeaderWindow::FbTreeViewHeaderWindow(wxWindow *win, wxWindowID id, FbTreeViewMainWindow *owner, const wxPoint& pos, const wxSize& size, long style, const wxString &name)
    : wxWindow(win, id, pos, size, style, name), m_owner(owner), m_sorted(0)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetBackgroundColour(wxSystemSettings::GetColour (wxSYS_COLOUR_BTNFACE));
}

FbTreeViewHeaderWindow::~FbTreeViewHeaderWindow()
{
}

int FbTreeViewHeaderWindow::GetFullWidth()
{
    int ww = 0;
    size_t count = m_columns.Count();
    for ( size_t i = 0; i < count; i++ ) ( ww += m_columns[i].GetWidth() );
    if (!ww) ww = 1;
    return ww;
}

void FbTreeViewHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
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
		if (abs(m_sorted) == (int)i + 1) sort = m_sorted > 0 ? wxHDR_SORT_ICON_DOWN : wxHDR_SORT_ICON_UP;

        wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0, sort, &params);
    }

    if (x < www) {
        wxRect rect(x, 0, www-x, h);
        wxRendererNative::Get().DrawHeaderButton(this, dc, rect);
    }
}

int FbTreeViewHeaderWindow::XToCol(int x)
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

void FbTreeViewHeaderWindow::OnMouse (wxMouseEvent &event)
{
    if (event.LeftUp() && m_sorted) {
        int col = XToCol(event.GetX());
        m_sorted = (abs(m_sorted) == col + 1) ? - m_sorted : col + 1;
        SendListEvent(wxEVT_COMMAND_LIST_COL_CLICK, event.GetPosition(), col);
        Refresh();
    }
	event.Skip();
}

void FbTreeViewHeaderWindow::OnSetFocus (wxFocusEvent &WXUNUSED(event))
{
    m_owner->SetFocus();
}

void FbTreeViewHeaderWindow::SendListEvent (wxEventType type, wxPoint pos, int colunm)
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
// FbTreeViewMainWindow implementation
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbTreeViewMainWindow, wxScrolledWindow)
    EVT_PAINT          (FbTreeViewMainWindow::OnPaint)
    EVT_ERASE_BACKGROUND(FbTreeViewMainWindow::OnEraseBackground) // to reduce flicker
    EVT_LEFT_DOWN(FbTreeViewMainWindow::OnLeftDown)
    EVT_MOUSE_EVENTS   (FbTreeViewMainWindow::OnMouse)
    EVT_CHAR           (FbTreeViewMainWindow::OnChar)
    EVT_SET_FOCUS      (FbTreeViewMainWindow::OnSetFocus)
    EVT_KILL_FOCUS     (FbTreeViewMainWindow::OnKillFocus)
    EVT_IDLE           (FbTreeViewMainWindow::OnIdle)
    EVT_SCROLLWIN      (FbTreeViewMainWindow::OnScroll)
    EVT_MOUSE_CAPTURE_LOST(FbTreeViewMainWindow::OnCaptureLost)
END_EVENT_TABLE()

// ---------------------------------------------------------------------------
// construction/destruction
// ---------------------------------------------------------------------------

FbTreeViewMainWindow::FbTreeViewMainWindow(
		FbTreeViewCtrl *parent,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxValidator &validator,
		const wxString& name) :
	wxScrolledWindow(parent, id, pos, size, style|wxVSCROLL, name),
	m_model(NULL),
	m_rowHeight(0)
{
	m_current = -1;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));

    SetFont(wxSystemSettings::GetFont (wxSYS_DEFAULT_GUI_FONT));

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
}


FbTreeViewMainWindow::~FbTreeViewMainWindow()
{
}

bool FbTreeViewMainWindow::SetFont(const wxFont& font)
{
    bool ok = wxScrolledWindow::SetFont(font);
    if (ok) {
        wxClientDC dc (this);
        dc.SetFont(font);
        int h = (int)(dc.GetCharHeight() + 2);
        m_rowHeight = h > FB_CHECKBOX_HEIGHT ? h : FB_CHECKBOX_HEIGHT;
    }
    return ok;
}

void FbTreeViewMainWindow::AdjustMyScrollbars()
{
    if (true) {
        int xUnit, yUnit;
        GetScrollPixelsPerUnit (&xUnit, &yUnit);
        if (yUnit == 0) yUnit = GetRowHeight();

        int y_pos = GetScrollPos (wxVERTICAL);
        SetScrollbars (0, yUnit, 0, GetRowCount(), 0, y_pos);
    } else {
        SetScrollbars (0, 0, 0, 0);
    }
}

int FbTreeViewMainWindow::GetClientCount()
{
    int hh, h = GetRowHeight();
    GetClientSize(NULL, &hh);
    return hh / h;
}

void FbTreeViewMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event))
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

    int h = GetRowHeight();

	wxRect rect(0, y_pos*h, ww, hh);

	FbColumnArray columns;

	if (m_model) m_model->DrawTree(dc, rect, columns, y_pos, h);
}

void FbTreeViewMainWindow::OnMouse (wxMouseEvent &event)
{
}

void FbTreeViewMainWindow::OnChar(wxKeyEvent &event)
{
    // send event to user code
    wxTreeEvent nevent (wxEVT_COMMAND_TREE_KEY_DOWN, 0 );
    nevent.SetInt(m_current);
    nevent.SetKeyEvent (event);
    if (SendEvent(0, NULL, &nevent)) return; // char event handled in user code

    if (!m_model) {
		event.Skip();
		return;
    }

    int pos = GetScrollPos (wxVERTICAL);
   	int hhh = GetClientCount();
   	int row = wxNOT_FOUND;

    if (event.m_controlDown) {
        bool ok = true;
        switch (event.GetKeyCode()) {
            case WXK_UP: {
                pos -= 1;
            } break;
            case WXK_DOWN: {
                pos += 1;
            } break;
            case WXK_HOME: {
                pos = 0;
            } break;
            case WXK_END: {
                pos = GetRowCount();
            } break;
            case WXK_PAGEUP: {
                pos -= hhh;
            } break;
            // <WXK_PAGEDOWN>: go to the next page
            case WXK_PAGEDOWN: {
                pos += hhh;
            } break;
            default: {
                ok = false;
            }
        }
        if (ok) {
            SetScrollPos(wxVERTICAL, pos);
            SetDirty();
            return;
        }
    }

    switch (event.GetKeyCode()) {
        case WXK_TAB: {
            Navigate( event.m_shiftDown ? wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward  );
            return;
        } break;
        case WXK_UP: {
            row = m_model->GoPriorRow();
        } break;
        case WXK_DOWN: {
            row = m_model->GoNextRow();
        } break;
        case WXK_HOME: {
        	row = m_model->GoFirstRow();
        } break;
        case WXK_END: {
        	row = m_model->GoLastRow();
        } break;
        case WXK_PAGEUP: {
        	row = m_model->GoPriorRow(hhh);
        } break;
        case WXK_PAGEDOWN: {
        	row = m_model->GoNextRow(hhh);
        } break;
        default: {
            event.Skip();
            return;
        }
    }

	if (row != wxNOT_FOUND) {
        if (pos > row) {
            SetScrollPos(wxVERTICAL, row);
        } else {
            if (row >= pos + hhh) {
                pos = (row - hhh) >= 0 ? (row - hhh + 1) : 0;
                SetScrollPos(wxVERTICAL, pos);
            }
        }
        SetDirty();
    }


/*

    // if no item current, select root
    bool curItemSet = false;
    if (!m_curItem) {
        if (! GetRootItem().IsOk()) return;
        m_curItem = (FbTreeViewItem*)GetRootItem().m_pItem;
        if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            m_curItem = (FbTreeViewItem*)GetFirstChild (m_curItem, cookie).m_pItem;
        }
        SelectItem(m_curItem, 0L, true);  // unselect others
        curItemSet = true;
    }

    // remember item at shift down
    if (HasFlag(wxTR_MULTIPLE) && event.ShiftDown()) {
        if (!m_shiftItem) m_shiftItem = m_curItem;
    }else{
        m_shiftItem = (FbTreeViewItem*)NULL;
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
            SelectItem (m_curItem, (FbTreeViewItem*)NULL, false);
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
        FbTreeViewItem *oldItem = m_curItem;
        m_curItem = (FbTreeViewItem*)newItem.m_pItem; // make the new item the current item
        RefreshLine (oldItem);
    }
*/
}

void FbTreeViewMainWindow::OnSetFocus (wxFocusEvent &event)
{
/*
    m_hasFocus = true;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
    event.Skip();
    SetDirty();
}

void FbTreeViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
/*
    m_hasFocus = false;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
    event.Skip();
    SetDirty();
}

void FbTreeViewMainWindow::OnIdle (wxIdleEvent &WXUNUSED(event))
{
    if (!m_dirty) return;
    m_dirty = false;

    AdjustMyScrollbars();
    Refresh();
}

void FbTreeViewMainWindow::OnScroll (wxScrollWinEvent& event)
{
//    // FIXME
//    #if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
//    wxScrolledWindow::OnScroll(event);
//    #else
//    HandleOnScroll( event );
//    #endif
	HandleOnScroll( event );
}

bool FbTreeViewMainWindow::SetBackgroundColour (const wxColour& colour)
{
    if (!wxWindow::SetBackgroundColour(colour)) return false;

    Refresh();
    return true;
}

bool FbTreeViewMainWindow::SetForegroundColour (const wxColour& colour)
{
    if (!wxWindow::SetForegroundColour(colour)) return false;

    Refresh();
    return true;
}

bool FbTreeViewMainWindow::SendEvent(wxEventType type, FbTreeViewItem *item, wxTreeEvent *event)
{
	wxTreeEvent nevent(type, 0);

    if (event == NULL) {
        event = &nevent;
        event->SetInt(0);
    }

    event->SetEventObject(m_owner);
    event->SetId(m_owner->GetId());
    if (item) event->SetItem(item);

    return m_owner->GetEventHandler()->ProcessEvent (*event);
}

void FbTreeViewMainWindow::SetFocus()
{
    wxWindow::SetFocus();
}

void FbTreeViewMainWindow::AssignModel(FbTreeModel * model)
{
	m_model = model;
	m_model->SetOwner(this);
	SetDirty();
}

void FbTreeViewMainWindow::OnLeftDown( wxMouseEvent& event )
{
	SetFocus();
}

//-----------------------------------------------------------------------------
//  FbTreeViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(FbTreeViewCtrl, wxControl);

BEGIN_EVENT_TABLE(FbTreeViewCtrl, wxControl)
    EVT_SIZE(FbTreeViewCtrl::OnSize)
END_EVENT_TABLE();

bool FbTreeViewCtrl::Create(wxWindow *parent, wxWindowID id,
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

    m_main_win = new FbTreeViewMainWindow (this, -1, wxPoint(0, 0), size, main_style, validator);

    m_header_win = new FbTreeViewHeaderWindow (this, -1, m_main_win, wxPoint(0, 0), wxDefaultSize, wxTAB_TRAVERSAL);

    DoHeaderLayout();

    return true;
}

void FbTreeViewCtrl::DoHeaderLayout()
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

void FbTreeViewCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoHeaderLayout();
}

bool FbTreeViewCtrl::SetBackgroundColour(const wxColour& colour)
{
    if (!m_main_win) return false;
    return m_main_win->SetBackgroundColour(colour);
}

bool FbTreeViewCtrl::SetForegroundColour(const wxColour& colour)
{
    if (!m_main_win) return false;
    return m_main_win->SetForegroundColour(colour);
}

wxSize FbTreeViewCtrl::DoGetBestSize() const
{
    return wxSize (200,200);
}

void FbTreeViewCtrl::SetFocus()
{
	m_main_win->SetFocus();
}

void FbTreeViewCtrl::Refresh(bool erase, const wxRect* rect)
{
    m_main_win->Refresh (erase, rect);
    m_header_win->Refresh (erase, rect);
}

bool FbTreeViewCtrl::SetFont(const wxFont& font)
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

void FbTreeViewCtrl::AddColumn(size_t model_column, const wxString& text, int width, int flag)
{
    if (m_header_win) m_header_win->AddColumn(FbTreeViewColumnInfo(model_column, text, width, flag));
}

void FbTreeViewCtrl::SetSortedColumn(int column)
{
    if (m_header_win) m_header_win->SetSortedColumn(column);
}

int FbTreeViewCtrl::GetSortedColumn()
{
    return m_header_win ? m_header_win->GetSortedColumn() : 0;
}

void FbTreeViewCtrl::AssignModel(FbTreeModel * model)
{
	m_main_win->AssignModel(model);
}

