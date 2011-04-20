#include "FbTreeView.h"

#include <wx/settings.h>
#include <wx/renderer.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>

#include "FbLogoBitmap.h"

class FbTreeViewItem;

class WXDLLEXPORT wxControlRenderer;

const wxChar* FbTreeViewCtrlNameStr = _T("treelistctrl");

class FbTreeViewColumnInfo: public wxObject
{
    public:
		FbTreeViewColumnInfo() {}

        FbTreeViewColumnInfo(
            size_t model_column,
            const wxString &text = wxEmptyString,
            int width = DEFAULT_COL_WIDTH,
            int flag = wxALIGN_LEFT
        ) : m_text(text), m_index(model_column), m_width(width), m_flag(flag) {};

        FbTreeViewColumnInfo(const FbTreeViewColumnInfo &info)
            : m_text(info.m_text), m_index(info.m_index), m_width(info.m_width), m_flag(info.m_flag) {};

        void Assign(FbTreeViewHeaderWindow * header, wxHeaderButtonParams &params) const;

        int GetIndex() const { return m_index; };

        int GetWidth() const { return m_width; };

        int GetFlag() const { return m_flag; };

        void SetWidth(int value) { m_width = value; };

    private:
        wxString m_text;
        size_t m_index;
        int m_width;
        int m_flag;

    private:
        friend class FbTreeViewMainWindow;
		DECLARE_CLASS(FbTreeViewColumnInfo);
};

IMPLEMENT_CLASS(FbTreeViewColumnInfo, wxObject)

static FbTreeViewColumnInfo wxInvalidTreeListColumnInfo;

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbTreeViewColumnInfo, FbArrayTreeViewColumn);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbArrayTreeViewColumn);

class  FbTreeViewHeaderWindow : public wxWindow
{
	protected:
		const wxCursor *m_currentCursor;
		const wxCursor *m_resizeCursor;
		bool m_isDragging;

		// column being resized
		int m_column;

		// divider line position in logical (unscrolled) coords
		int m_currentX;

		// minimal position beyond which the divider line can't be dragged in logical coords
		int m_minX;
		int m_maxX;

		// needs refresh
		bool m_dirty;

		void RefreshColLabel(int col);
		void SetColumnWidth (int column, int width);

	public:
		FbTreeViewHeaderWindow( wxWindow *win,
								wxWindowID id,
								FbTreeViewMainWindow *owner,
								const wxPoint &pos = wxDefaultPosition,
								const wxSize &size = wxDefaultSize,
								long style = 0,
								const wxString &name = wxT("FbTreeViewctrlcolumntitles") );

		virtual ~FbTreeViewHeaderWindow();

		void DoDrawRect( wxDC *dc, int x, int y, int w, int h );

		void DrawCurrent();

        void AddColumn(const FbTreeViewColumnInfo & info) { m_columns.Add(info); };

        size_t GetColumnCount() const { return m_columns.Count(); };

        void EmptyColumns() { m_columns.Empty(); }

        const FbTreeViewColumnInfo & GetColumn (size_t column) const {
            wxCHECK_MSG (column < GetColumnCount(), wxInvalidTreeListColumnInfo, _T("Invalid column"));
            return m_columns[column];
        }

        int GetColumnWidth(size_t column) { return GetColumn(column).GetWidth(); };

        int GetColumnIndex(size_t column) { return GetColumn(column).GetIndex(); };

        int GetFullWidth();

        int XToCol(int x);

        int GetSortedColumn() { return m_sorted; }

        void SetSortedColumn(int column) { m_sorted = column; }

		void GetColumnInfo(FbColumnArray &columns, int width = 0);

	private:
		FbTreeViewMainWindow * m_owner;
		FbArrayTreeViewColumn m_columns;
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
				   long style = fbTR_DEFAULT_STYLE,
				   const wxValidator &validator = wxDefaultValidator,
				   const wxString& name = wxT("FbTreeViewmainwindow"));

		virtual ~FbTreeViewMainWindow();

		virtual bool SetBackgroundColour (const wxColour& colour);
		virtual bool SetForegroundColour (const wxColour& colour);

		virtual void SetFocus();
        virtual bool SetFont(const wxFont& font);

	public:
		bool SendEvent(wxEventType type, FbTreeViewItem *item = NULL, wxTreeEvent *event = NULL);  // returns true if processed
		size_t GetRowCount() { return m_model ? m_count = m_model->GetRowCount() : m_count; }
		void Repaint() { m_dirty = true; }
		void AssignModel(FbModel * model);
		FbModel * GetModel() { return m_model; }
		bool ShowScrollbar() { return GetRowCount() > GetClientCount(); }
		bool FindAt(const wxPoint &point, bool select = false);
		int GetRowHeight() { return m_rowHeight; };

	private:
		size_t GetClientCount();
        void AdjustMyScrollbars();

	private:
		FbTreeViewCtrl* m_owner;
        FbModel * m_model;
        wxPen m_dottedPen;
		int m_current;
        bool m_dirty;
        int m_rowHeight;
        bool m_focused;
        size_t m_count;

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

    m_resizeCursor = new wxCursor(wxCURSOR_SIZEWE);
    m_currentCursor = (wxCursor *) NULL;
    m_isDragging = false;
    m_dirty = false;
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
	int w = www;
	if (m_owner && m_owner->ShowScrollbar())
		w -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    int x = 0;
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( size_t i = 0; i < count && x < www; i++ ) {
        wxHeaderButtonParams params;
        GetColumn(i).Assign(this, params);
		int index = GetColumn(i).GetIndex();

        int wCol = GetColumnWidth(i) * w / ww;
        if (i == count - 1) wCol = w - x;
        wxRect rect(x, 0, wCol, h);
        x += wCol;

		wxHeaderSortIconType sort = wxHDR_SORT_ICON_NONE;
		if (abs(m_sorted) == index + 1) sort = m_sorted > 0 ? wxHDR_SORT_ICON_DOWN : wxHDR_SORT_ICON_UP;

        wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0, sort, &params);
    }

    if (x < www) {
        wxRect rect(x, 0, www-x, h);
        wxRendererNative::Get().DrawHeaderButton(this, dc, rect);
    }
}

void FbTreeViewHeaderWindow::GetColumnInfo(FbColumnArray &columns, int ww)
{
	if (ww == 0) {
		GetClientSize( &ww, NULL );
		if (m_owner && m_owner->ShowScrollbar())
			ww -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
	}

    int x = 0;
    int www = GetFullWidth();
    size_t count = GetColumnCount();
    for ( size_t i = 0; i < count; i++ ) {
        FbTreeViewColumnInfo & col = m_columns[i];
		int w = col.GetWidth() * ww / www;
		if (i == count - 1) w = ww - x;
		columns.Add(FbColumnInfo(col.GetIndex(), w, col.GetFlag()));
        x += w;
	}
}

int FbTreeViewHeaderWindow::XToCol(int x)
{
    int w, left = 0;
    m_owner->GetClientSize( &w, NULL );
    int ww = GetFullWidth();
    size_t count = GetColumnCount();
    for ( size_t col = 0; col < count; col++ ) {
        left += GetColumnWidth(col) * w / ww;
        if (x < left) return col;
    }
    return count - 1;
}

void FbTreeViewHeaderWindow::RefreshColLabel(int col)
{
	FbColumnArray columns;
	GetColumnInfo(columns);

	if (col < 0) return;
	if (col >= columns.Count()) return;

    int x = 0;
    int width = 0;
    int idx = 0;
    do {
    	FbColumnInfo & column = columns[idx];
        x += width;
        width = column.GetWidth();
    } while (++idx <= col);

    RefreshRect(wxRect(x, 0, width, GetSize().GetHeight()));
}

void FbTreeViewHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    ClientToScreen (&x1, &y1);

    int x2 = m_currentX-1;
#ifdef __WXMSW__
    ++x2; // but why ????
#endif
    int y2 = 0;
    m_owner->GetClientSize( NULL, &y2 );
    m_owner->ClientToScreen( &x2, &y2 );

    wxScreenDC dc;
    dc.SetLogicalFunction (wxINVERT);
    dc.SetPen (wxPen (*wxBLACK, 2, wxSOLID));
    dc.SetBrush (*wxTRANSPARENT_BRUSH);

    dc.DrawLine (x1, y1, x2, y2);
    dc.SetLogicalFunction (wxCOPY);
    dc.SetPen (wxNullPen);
    dc.SetBrush (wxNullBrush);
}

void FbTreeViewHeaderWindow::OnMouse (wxMouseEvent &event)
{
    int x = event.GetX();

    if (m_isDragging) {

        // we don't draw the line beyond our window, but we allow dragging it there
        int w = 0;
        m_owner->GetClientSize( &w, NULL );
        w -= 6;

        // erase the line if it was drawn
        if (m_currentX < w) DrawCurrent();

        if (event.ButtonUp()) {
            m_isDragging = false;
            if (HasCapture()) ReleaseMouse();
            m_dirty = true;
            SetColumnWidth (m_column, m_currentX - m_minX);
            Refresh();
        } else {
            m_currentX = wxMax (m_minX + 7, x);

            // draw in the new location
            if (m_currentX < w) DrawCurrent();
        }

    } else { // not dragging

        m_minX = 0;
        bool hit_border = false;

        // end of the current column
        int xpos = 0;

		FbColumnArray columns;
		GetColumnInfo(columns);

        // find the column where this event occured
        size_t count = columns.Count() - 1;
		for (size_t i = 0; i < count; i++){
			FbColumnInfo & column = columns[i];
            xpos += column.GetWidth();
            m_column = i;

            if (abs (x-xpos) < 3) {
                // near the column border
                hit_border = true;
                break;
            }

            if (x < xpos) {
                // inside the column
                break;
            }

            m_minX = xpos;
		}

		m_maxX = 0;

		if (hit_border && event.LeftDown()) {
			m_isDragging = true;
			CaptureMouse();
			m_currentX = x;
			DrawCurrent();
		} else if (event.LeftUp() && m_sorted) {
			int col = XToCol(event.GetX());
			int index = GetColumn(col).GetIndex();
			m_sorted = (abs(m_sorted) == index + 1) ? - m_sorted : index + 1;
			SendListEvent(wxEVT_COMMAND_LIST_COL_CLICK, event.GetPosition(), col);
			Refresh();
        } else if (event.Moving()) {
            bool setCursor;
            if (hit_border) {
                setCursor = m_currentCursor == wxSTANDARD_CURSOR;
                m_currentCursor = m_resizeCursor;
            } else {
                setCursor = m_currentCursor != wxSTANDARD_CURSOR;
                m_currentCursor = wxSTANDARD_CURSOR;
            }
            if (setCursor) SetCursor (*m_currentCursor);
        }
    }
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

void FbTreeViewHeaderWindow::SetColumnWidth (int column, int width)
{
    wxCHECK_RET ((column >= 0) && (column < GetColumnCount()), _T("Invalid column"));

	int ww = 0;
	int www = 0;

	FbColumnArray columns;
	GetColumnInfo(columns);
	size_t count = columns.Count();
	for (size_t i = 0; i < count; i++){
		int w = columns[i].GetWidth();
		if (i < column) {
			m_columns[i].SetWidth(w);
		} else if (i == column) {
			m_columns[i].SetWidth(width);
			www = w - width;
		} else if (i > column) {
			www += w;
			ww += w;
		}
	}

	if (ww == 0) ww = 1;

	for (size_t i = column + 1; i < count; i++){
		int w = columns[i].GetWidth() * www / ww;
		if (w < 7) w = 7;
		m_columns[i].SetWidth(w);
	}

    m_owner->Refresh();
}

// ---------------------------------------------------------------------------
// FbTreeViewMainWindow implementation
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbTreeViewMainWindow, wxScrolledWindow)
    EVT_PAINT          (FbTreeViewMainWindow::OnPaint)
    EVT_ERASE_BACKGROUND(FbTreeViewMainWindow::OnEraseBackground) // to reduce flicker
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
	m_rowHeight(0),
	m_focused(false),
	m_count(0)
{
	m_current = -1;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));

    SetFont(wxSystemSettings::GetFont (wxSYS_DEFAULT_GUI_FONT));

    SetCursor(*wxHOURGLASS_CURSOR);

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
	wxDELETE(m_model);
}

bool FbTreeViewMainWindow::SetFont(const wxFont& font)
{
    bool ok = wxScrolledWindow::SetFont(font);
    if (ok) {
        wxClientDC dc (this);
        dc.SetFont(font);
        int h = dc.GetCharHeight();
        m_rowHeight = h > FB_CHECKBOX_HEIGHT ? h : FB_CHECKBOX_HEIGHT;
        m_rowHeight += 4;
    }
    return ok;
}

void FbTreeViewMainWindow::AdjustMyScrollbars()
{
	SetScrollbars(0, GetRowHeight(), 0, GetRowCount(), 0, GetScrollPos(wxVERTICAL));
}

size_t FbTreeViewMainWindow::GetClientCount()
{
    int hh, h = GetRowHeight();
    GetClientSize(NULL, &hh);
    return (size_t)(hh / h);
}

void FbTreeViewMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc (this);
    DoPrepareDC (dc);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetFont(GetFont());
    dc.SetBackground(brush);
    dc.Clear();

    int pos = GetScrollPos(wxVERTICAL);

    int ww, hh, yy;
    int h = GetRowHeight();
    GetClientSize(&ww, &hh);
	CalcUnscrolledPosition(0, 0, NULL, &yy);
	wxRect rect(0, yy, ww, hh);

	FbColumnArray columns;
	FbTreeViewHeaderWindow * header = m_owner->GetHeaderWindow();
	if (header) {
		header->GetColumnInfo(columns, ww);
	} else {
		columns.Add(FbColumnInfo(0, ww, wxALIGN_LEFT));
	}

	if (m_model) {
		m_model->SetFocused(m_focused);
		m_model->DrawTree(dc, rect, columns, pos, h);
	}
}

void FbTreeViewMainWindow::OnChar(wxKeyEvent &event)
{
    if (!m_model) { event.Skip(); return; }

    int pos = GetScrollPos (wxVERTICAL);
   	int hhh = GetClientCount();

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
            Repaint();
            return;
        }
    }

    // remember item at shift down
    if (HasFlag(fbTR_MULTIPLE) && event.GetKeyCode() != ' ') {
		m_model->SetShift(event.ShiftDown());
    }

   	int old = m_model->GetPosition();
   	int row = 0;
    switch (event.GetKeyCode()) {
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
        case WXK_RETURN: {
            SendEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED);
        } break;
        case ' ': {
        	if (HasFlag(fbTR_CHECKBOX)) {
        		if (HasFlag(fbTR_MULTIPLE)) {
					m_model->MultiplyCheck();
        		} else {
					m_model->SingleCheck();
        		}
				Repaint();
        	} else {
        		event.Skip();
			}
            return;
        }break;
        default: {
            event.Skip();
            return;
        }
    }

	if (row) {
        if (row <= pos) {
            SetScrollPos(wxVERTICAL, row - 1);
        } else {
            if (row > pos + hhh) {
                int pos = row > hhh ? row - hhh : 0;
                SetScrollPos(wxVERTICAL, pos);
            }
        }
        if (row != old) SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
        Repaint();
		return ;
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

bool FbTreeViewMainWindow::FindAt(const wxPoint &point, bool select)
{
	int x = point.x;
	int y = point.y;
	int h = GetRowHeight();

	if (m_model && x>=0 && y>=0) {
		CalcUnscrolledPosition(x, y, &x, &y);
		size_t row = (size_t)(y / h) + 1;
		size_t old_pos = m_model->GetPosition();
		size_t new_pos = m_model->FindRow(row, select);
		if (select && (old_pos != new_pos)) {
			SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
			Refresh();
		}
		return new_pos;
	}
	return false;
}

void FbTreeViewMainWindow::OnMouse (wxMouseEvent &event)
{
    if (!m_model) { event.Skip(); return; }

    // send event to user code
    if (m_owner->GetEventHandler()->ProcessEvent(event)) return; // handled (and not skipped) in user code

    // set focus if window clicked
    if (event.LeftDown() || event.MiddleDown() || event.RightDown()) SetFocus();

    if (event.Dragging()) {
		event.Skip();
		return;
	}

	if (event.GetEventType() == wxEVT_MOUSEWHEEL) {
		event.Skip();
		return;
	}

    // remember item at shift down
    if (HasFlag(fbTR_MULTIPLE) && event.LeftDown()) {
    	if (event.ControlDown()) {
			m_model->InitCtrls();
    	} else {
			m_model->SetShift(event.ShiftDown());
    	}
    }

	int x = event.GetX();
	int y = event.GetY();
	int h = GetRowHeight();

	if (m_model && x>=0 && y>=0) {
		CalcUnscrolledPosition(x, y, &x, &y);
		size_t row = (size_t)(y / h) + 1;
		if (event.LeftDown() || event.LeftDClick() || event.MiddleDown() || event.RightDown()) {
			int level;
			FbModelItem item = m_model->GetData(row, level);
			if (!item) { event.Skip(); return; }

			int left = FB_CHECKBOX_WIDTH * level;
			int right = left + FB_CHECKBOX_WIDTH + 2;

			if (HasFlag(fbTR_CHECKBOX) && left <= x && x <= right) {
				size_t pos = m_model->FindRow(row, false);
				m_model->SingleCheck(pos);
			} else {
				size_t old_pos = m_model->GetPosition();
				size_t new_pos = m_model->FindRow(row, true);
				if (old_pos != new_pos) SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
				if (event.LeftDClick()) SendEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED);
				if (event.ControlDown()) m_model->InvertCtrl();
			}
			Repaint();
			return;
		}
	}

    // generate click & menu events
    if (event.MiddleDown()) {
        SendEvent(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK);
    }
    if (event.RightDown()) {
        SendEvent(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK);
    }
    if (event.RightUp()) {
        wxTreeEvent nevent(wxEVT_COMMAND_TREE_ITEM_MENU, 0);
        nevent.SetPoint(wxPoint(event.GetX(), event.GetY()));
        SendEvent(0, NULL, &nevent);
    }

	event.Skip();

/*
void wxTreeListMainWindow::OnMouse (wxMouseEvent &event) {
bool mayDrag = true;
bool maySelect = true;  // may change selection
bool mayClick = true;  // may process DOWN clicks to expand, send click events
bool mayDoubleClick = true;  // implies mayClick
bool bSkip = true;

    // send event to user code
    if (m_owner->GetEventHandler()->ProcessEvent(event)) return; // handled (and not skipped) in user code
    if (!m_rootItem) return;

    // set focus if window clicked
    if (event.LeftDown() || event.MiddleDown() || event.RightDown()) SetFocus();


// ---------- DETERMINE EVENT ----------

	wxPoint p = wxPoint (event.GetX(), event.GetY());
    int flags = 0;
    wxTreeListItem *item = m_rootItem->HitTest (CalcUnscrolledPosition (p),
                                                this, flags, m_curColumn, 0);
    bool bCrosshair = (item && item->HasPlus() && (flags & wxTREE_HITTEST_ONITEMBUTTON));
    // we were dragging
    if (m_isDragging) {
        maySelect = mayDoubleClick = false;
    }
    // we are starting or continuing to drag
    if (event.Dragging()) {
        maySelect = mayDoubleClick = mayClick = false;
    }
    // crosshair area is special
    if (bCrosshair) {
        // left click does not select
        if (event.LeftDown()) maySelect = false;
        // double click is ignored
        mayDoubleClick = false;
    }


// HANDLE SIMPLE-CLICKS (selection change, contextual menu)
    if (mayClick) {

		//DENIS KANDRASHIN 2009-06-10 - BEGIN - Mouse click on image
		int X = CalcUnscrolledPosition (p).x;
		if (item && event.LeftDown() &&  X < item->GetX() + MARGIN + 8) {
			SendEvent(wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, item);
			return;
		}
		//DENIS - END

        // left-click on haircross is expand (and no select)
        if (bCrosshair && event.LeftDown()) {

            bSkip = false;

            // note that we only toggle the item for a single click, double
            // click on the button doesn't do anything
            Toggle (item);
        }

        // is there a selection change ? normally left and right down-click
        //  change selection, but there are special cases:
        if (maySelect && (
            // click on already selected item: to allow drag of multiple items,
            //  change selection on button up
            ((event.LeftUp() || event.RightUp()) && item != NULL && item->IsSelected() && item != m_curItem)
            // normal clicks, act already on button down
         || ((event.LeftDown() || event.RightDown()) && (item == NULL || ! item->IsSelected()))
        )) {

            bSkip = false;

            // set / remember item at shift down before current item gets changed
            if (event.LeftDown() && HasFlag(wxTR_MULTIPLE) && event.ShiftDown())  {
                if (!m_shiftItem) m_shiftItem = m_curItem;
            }else{
                m_shiftItem = (wxTreeListItem*)NULL;
            }

            // how is selection altered
            // keep or discard already selected ?
            bool unselect_others = ! (HasFlag(wxTR_MULTIPLE) && (
                event.ShiftDown()
             || event.ControlDown()
            ));

            // check is selection change is not vetoed
            if (SelectItem(item, m_shiftItem, unselect_others)) {

                // make the new item the current item
                EnsureVisible (item);
                m_curItem = item;
            }

        // no selection change, then we might edit
        } else {
            if (event.LeftDown())
                m_lastOnSame = (item == m_curItem);
        }

        // generate click & menu events
        if (event.MiddleDown()) {
            bSkip = false;
            SendEvent(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, item);
        }
        if (event.RightDown()) {
            bSkip = false;
            SendEvent(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, item);
        }
        if (event.RightUp()) {
            wxTreeEvent nevent(wxEVT_COMMAND_TREE_ITEM_MENU, 0);
            nevent.SetPoint(p);
            nevent.SetInt(m_curColumn);
            SendEvent(0, item, &nevent);
        }

        // if 2nd left click finishes on same item, will edit it
        if (m_lastOnSame && event.LeftUp()) {
            if ((item == m_curItem) && (m_curColumn != -1) &&
                (m_owner->GetHeaderWindow()->IsColumnEditable (m_curColumn)) &&
                (flags & (wxTREE_HITTEST_ONITEMLABEL | wxTREE_HITTEST_ONITEMCOLUMN))
            ){
                m_editTimer->Start (RENAME_TIMER_TICKS, wxTIMER_ONE_SHOT);
                bSkip = false;
            }
            m_lastOnSame = false;
        }
    }


// ----------  HANDLE DOUBLE-CLICKS  ----------
    if (mayClick && mayDoubleClick && event.LeftDClick()) {

        bSkip = false;

        // double clicking should not start editing the item label
        m_editTimer->Stop();
        m_lastOnSame = false;

        // selection reset to that single item which was double-clicked
        if (SelectItem(item, 0L, true)) {  // unselect others --return false if vetoed

            // selection change not vetoed, send activate event
            if (! SendEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, item)) {

                // if the user code didn't process the activate event,
                // handle it ourselves by toggling the item when it is
                // double clicked
                if (item && item->HasPlus()) Toggle(item);
            }
        }
    }


// ----------  HANDLE DRAGGING  ----------
// NOTE: drag itself makes no change to selection
    if (mayDrag) {  // actually this is always true

        // CASE 1: we were dragging => continue, end, abort
        if (m_isDragging) {

            // CASE 1.1: click aborts drag:
            if (event.LeftDown() || event.MiddleDown() || event.RightDown()) {

                bSkip = false;

                // stop dragging
                m_isDragStarted = m_isDragging = false;
                if (HasCapture()) ReleaseMouse();
                RefreshSelected();

            // CASE 1.2: still dragging
            } else if (event.Dragging()) {

                ;; // nothing to do

            // CASE 1.3: dragging now ends normally
            } else {

                bSkip = false;

                // stop dragging
                m_isDragStarted = m_isDragging = false;
                if (HasCapture()) ReleaseMouse();
                RefreshSelected();

                // send drag end event
                wxTreeEvent event(wxEVT_COMMAND_TREE_END_DRAG, 0);
                event.SetPoint(p);
                event.SetInt(m_curColumn);
                SendEvent(0, item, &event);
            }

        // CASE 2: not were not dragging => continue, start
        } else if (event.Dragging()) {

            // We will really start dragging if we've moved beyond a few pixels
            if (m_isDragStarted) {
                const int tolerance = 3;
                int dx = abs(p.x - m_dragStartPos.x);
                int dy = abs(p.y - m_dragStartPos.y);
                if (dx <= tolerance && dy <= tolerance)
                    return;
            // determine drag start
            } else {
                m_dragStartPos = p;
                m_dragCol = m_curColumn;
                m_dragItem = item;
                m_isDragStarted = true;
                return;
            }

            bSkip = false;

            // we are now dragging
            m_isDragging = true;
            RefreshSelected();
            CaptureMouse(); // TODO: usefulness unclear

            wxTreeEvent nevent(event.LeftIsDown()
                                  ? wxEVT_COMMAND_TREE_BEGIN_DRAG
                                  : wxEVT_COMMAND_TREE_BEGIN_RDRAG, 0);
            nevent.SetPoint(p);
            nevent.SetInt(m_dragCol);
            nevent.Veto();
            SendEvent(0, m_dragItem, &nevent);
        }
    }


    if (bSkip) event.Skip();
*/
}

void FbTreeViewMainWindow::OnSetFocus (wxFocusEvent &event)
{
/*
    m_hasFocus = true;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
	m_focused = true;
    event.Skip();
    Repaint();
}

void FbTreeViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
/*
    m_hasFocus = false;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
*/
	m_focused = false;
    event.Skip();
    Repaint();
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
	#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    wxScrolledWindow::OnScroll(event);
	#else
    HandleOnScroll( event );
	#endif
	Repaint();
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

void FbTreeViewMainWindow::AssignModel(FbModel * model)
{
    SetCursor(model ? *wxSTANDARD_CURSOR : *wxHOURGLASS_CURSOR);
	wxDELETE(m_model);
	m_model = model;
	Repaint();
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
	main_style |= wxWANTS_CHARS;

    long ctrl_style = style & ~(wxVSCROLL|wxHSCROLL);
    ctrl_style |= wxTAB_TRAVERSAL;

    if (!wxControl::Create(parent, id, pos, size, ctrl_style, validator, name)) {
       return false;
    }

    m_main_win = new FbTreeViewMainWindow (this, -1, wxPoint(0, 0), size, main_style, validator);

   	if (HasFlag(fbTR_NO_HEADER )) {
   		m_header_win = NULL;
   	} else {
   		m_header_win = new FbTreeViewHeaderWindow (this, -1, m_main_win, wxPoint(0, 0), wxDefaultSize, wxTAB_TRAVERSAL);
   	}

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
    }
    if (m_main_win) {
		m_main_win->Repaint();
        m_main_win->SetSize (0, h, x, y - h);
    }
    if (m_header_win) {
        m_header_win->SetSize (0, 0, x, h);
        m_header_win->Refresh();
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

void FbTreeViewCtrl::SetFocus()
{
	m_main_win->SetFocus();
}

void FbTreeViewCtrl::Refresh(bool erase, const wxRect* rect)
{
	m_main_win->Repaint();
	if (m_header_win) m_header_win->Refresh (erase, rect);
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

void FbTreeViewCtrl::AssignModel(FbModel * model)
{
    if (m_main_win) {
		if (model) model->SetOwner(this);
        m_main_win->AssignModel(model);
        m_main_win->SetScrollPos(wxVERTICAL, 0);
		m_main_win->SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
    }
	Refresh();
}

FbModel * FbTreeViewCtrl::GetModel() const
{
	return m_main_win ? m_main_win->GetModel() : NULL;
}

FbModelItem FbTreeViewCtrl::GetCurrent() const
{
	FbModel * model = GetModel();
	return model ? model->GetCurrent() : FbModelItem();
}

wxString FbTreeViewCtrl::GetCurrentText() const
{
	FbModel * model = GetModel();
	if (model) {
		size_t position = model->GetPosition();
	    FbModelItem item = model->GetData(position);
	    if (item) return item[0];
	}
	return wxEmptyString;
}

void FbTreeViewCtrl::EmptyColumns()
{
    if (m_header_win) m_header_win->EmptyColumns();
}

wxString FbTreeViewCtrl::GetText()
{
	FbModel * model = GetModel();
	if (model) {
		wxArrayInt columns;
		GetColumns(columns);
		return model->GetText(columns);
	} else {
		return wxEmptyString;
	}
}

void FbTreeViewCtrl::SelectAll(bool value)
{
	FbModel * model = GetModel();
	if (model) model->SelectAll(value);
	Refresh();
}

void FbTreeViewCtrl::Append(FbModelData * data)
{
	FbModel * model = GetModel();
	if (model) model->Append(data); else delete data;
	if (m_main_win) m_main_win->SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
	Refresh();
}

void FbTreeViewCtrl::Replace(FbModelData * data)
{
	FbModel * model = GetModel();
	if (model) model->Replace(data); else delete data;
	if (m_main_win) m_main_win->SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
	Refresh();
}

void FbTreeViewCtrl::Delete()
{
	FbModel * model = GetModel();
	if (model) model->Delete();
	if (m_main_win) m_main_win->SendEvent(wxEVT_COMMAND_TREE_SEL_CHANGED);
	Refresh();
}

int FbTreeViewCtrl::GetBook() const
{
	FbModel * model = GetModel();
	if (model) {
		FbModelItem item = model->GetCurrent();
		if (item) return (&item)->GetBook();
	}
	return 0;
}

void FbTreeViewCtrl::GetColumns(wxArrayInt &columns) const
{
	columns.Empty();
    if (m_header_win) {
    	size_t count = m_header_win->GetColumnCount();
    	for (size_t i = 0; i < count; i++) {
    		size_t index = m_header_win->GetColumnIndex(i);
    		columns.Add(index);
		}
    }
}

bool FbTreeViewCtrl::FindAt(const wxPoint &point, bool select)
{
	return m_main_win->FindAt(point, select);
}

int FbTreeViewCtrl::GetRowHeight()
{
	return m_main_win->GetRowHeight();
}
