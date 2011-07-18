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
			int flag = wxALIGN_LEFT,
			int fixed = 0
		) : m_text(text), m_index(model_column), m_size(width), m_width(0), m_flag(flag), m_fixed(fixed) {};

		FbTreeViewColumnInfo(const FbTreeViewColumnInfo &info)
			: m_text(info.m_text), m_index(info.m_index), m_size(info.m_size), m_width(info.m_width), m_flag(info.m_flag), m_fixed(info.m_fixed) {};

		void Assign(FbTreeViewHeaderWindow * header, wxHeaderButtonParams &params) const;

		int GetIndex() const { return m_index; };

		int GetSize() const { return m_size; };

		int GetWidth() const { return m_width; };

		int GetFlag() const { return m_flag; };

		int GetFixed() const { return m_fixed; };

		void SetWidth(int value) { m_width = value; };

		void SetSize(int value) { m_size = value; };

	private:
		wxString m_text;
		size_t m_index;
		int m_size;
		int m_width;
		int m_flag;
		int m_fixed;

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
		void CheckSize(wxDC & dc);

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

	protected:
		void OnSize( wxSizeEvent& event );
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
	EVT_SIZE(FbTreeViewHeaderWindow::OnSize)
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
	wxDELETE(m_resizeCursor);
}

void FbTreeViewHeaderWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
	CheckSize(wxClientDC(this));
}

void FbTreeViewHeaderWindow::CheckSize(wxDC & dc)
{
	int count = m_columns.Count();
	if (!count) return;

	// Get client width
	int www;
	GetClientSize( &www, NULL );
	if (m_owner && m_owner->ShowScrollbar())
		www -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

	// Check: if width don't change, then exit
	int ww = www;
	for ( int i = 0; i < count; i++ ) {
		ww -= m_columns[i].GetWidth();
	}
	if (-2 < ww && ww < 2) return;

	// Check if width is too small
	if (www <= 4 * count) {
		int x = www / count; 
		int w = www; 
		for ( int i = 1; i < count; i++ ) { 
			m_columns[i].SetWidth(x);
			w -= x;
		}
		m_columns[0].SetWidth(w);
		return;
	}

	// Get the char width and minimum columns width
	int cw = dc.GetCharWidth() * 2;
	int fixedWidth = 0, fixedCount = 0, fixedSize = 0;
	int floatWidth = 0, floatCount = 0, floatSize = 0;

	for ( int i = 0; i < count; i++ ) {
		int s = m_columns[i].GetSize();
		if (s > 0) {
			fixedWidth += s * cw; 
			fixedSize += s;
			fixedCount++;
		} else {
			floatWidth -= s * cw;
			floatSize -= s;
			floatCount++;
		}
	}

	double fixed_x, float_x;
	if (www < fixedWidth + floatWidth) {
		fixed_x = (double) www / (fixedSize + floatSize);
		float_x = fixed_x;
	} else {
		fixed_x = cw;
		float_x = (double) (www - fixedWidth) / floatSize;
	}

	// Define new column width
	int w = www; 
	for ( int i = 1; i < count; i++ ) { 
		int s = m_columns[i].GetSize();
		int x = s > 0 ? fixed_x * s : - float_x * s;
		m_columns[i].SetWidth(x);
		w -= x;
	}
	m_columns[0].SetWidth(w);
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

	CheckSize(dc);

	int x = 0;
	int h, www;
	GetClientSize(&www, &h);
	size_t count = GetColumnCount();
	for ( size_t i = 0; i < count; i++ ) {
		wxHeaderButtonParams params;
		GetColumn(i).Assign(this, params);
		int index = GetColumn(i).GetIndex();

		int wCol = GetColumnWidth(i);
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
	size_t count = GetColumnCount();
	for ( size_t i = 0; i < count; i++ ) {
		FbTreeViewColumnInfo & col = m_columns[i];
		columns.Add(FbColumnInfo(col.GetIndex(), col.GetWidth(), col.GetFlag()));
	}
}

int FbTreeViewHeaderWindow::XToCol(int x)
{
	int left = 0;
	size_t count = GetColumnCount();
	for ( size_t col = 0; col < count; col++ ) {
		left += GetColumnWidth(col);
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
			m_currentX = wxMax (2, x);

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
			m_column = i + 1;

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
			m_minX = x;
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

void FbTreeViewHeaderWindow::SetColumnWidth (int column, int delta)
{
	int count = GetColumnCount();
	if (column <= 0 || column > count) return;

	FbTreeViewColumnInfo & info = m_columns[column];
	int width = info.GetWidth() - delta;
	if (width < 4 ) width = 4;

	int old_size = info.GetSize();
	int new_size = (double) width / info.GetWidth() * info.GetSize();
	if (new_size == 0) new_size = (info.GetSize() > 0) ? 1 : -1;

	if (new_size < 0) {
		if (old_size < new_size) {
			int delta = old_size - new_size;
			for (int i = column - 1; i >= 0; i-- ) {
				FbTreeViewColumnInfo & info = m_columns[i];
				if (info.GetSize() < 0) {
					int s = info.GetSize() + delta;
					info.SetSize(s);
					break;
				}
			}
		} else {
			int delta = new_size - old_size;
			for (int i = column - 1; i >= 0; i-- ) {
				if (delta >= 0) break;
				FbTreeViewColumnInfo & info = m_columns[i];
				int size = info.GetSize();
				if (size < 0) {
					int s = wxMin(size - delta, -1);
					delta += s - info.GetSize();
					info.SetSize(s);
				}
			}
		}
	}

	info.SetSize( new_size );
	info.SetWidth(-100);

	CheckSize(wxClientDC(this));
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
	wxScrolledWindow(parent, id, pos, size, style|wxVSCROLL|wxFULL_REPAINT_ON_RESIZE, name),
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

	wxClientDC dc (this);
	dc.SetFont(font);
	int h = dc.GetCharHeight();
	m_rowHeight = h > FB_CHECKBOX_HEIGHT ? h : FB_CHECKBOX_HEIGHT;
	m_rowHeight += 4;

	return ok;
}

void FbTreeViewMainWindow::AdjustMyScrollbars()
{
	int pos = m_owner->GetScrollPos();
	if (pos) {
		m_owner->SetScrollPos(0);
	} else {
		pos = GetScrollPos(wxVERTICAL);
	}
	SetScrollbars(0, GetRowHeight(), 0, GetRowCount(), 0, pos);
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
	if (event.GetKeyCode() == WXK_TAB) {
		wxWindow * owner = GetParent()->GetParent();
		wxNavigationKeyEvent nevent;
		nevent.SetWindowChange( event.ControlDown() );
		nevent.SetDirection( !event.ShiftDown() );
		nevent.SetEventObject( owner );
		nevent.SetCurrentFocus( GetParent() );
		if (!owner->GetEventHandler()->ProcessEvent( nevent )) event.Skip();
		return;
	} 

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
		case WXK_TAB: {
			wxWindow * owner = GetParent()->GetParent();
			wxNavigationKeyEvent nevent;
			nevent.SetWindowChange( event.ControlDown() );
			nevent.SetDirection( !event.ShiftDown() );
			nevent.SetEventObject( owner );
			nevent.SetCurrentFocus( GetParent() );
			if (owner->GetEventHandler()->ProcessEvent( nevent )) return;
			event.Skip();
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
		case WXK_RETURN: {
			SendEvent(wxEVT_COMMAND_TREE_ITEM_ACTIVATED);
		} break;
        case '*':
        case '+':
		case WXK_NUMPAD_MULTIPLY:
        case WXK_MULTIPLY:
		case WXK_NUMPAD_ADD:
        case WXK_ADD: {
			if (HasFlag(fbTR_DIRECTORY)) {
				bool ok = m_model->GetCurrent().Expand(true);
				if (ok) {
					AdjustMyScrollbars();
					Repaint();
				}
			}
        } break;
        case '-':
		case WXK_NUMPAD_SUBTRACT:
        case WXK_SUBTRACT: {
			if (HasFlag(fbTR_DIRECTORY)) {
				bool ok = m_model->GetCurrent().Expand(false);
				if (ok) {
					AdjustMyScrollbars();
					Repaint();
				}
			}
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
			} else if (HasFlag(fbTR_DIRECTORY) && left <= x && x <= right) {
				size_t pos = m_model->FindRow(row, false);
				bool ok = item.Expand(!item.IsExpanded());
				if (ok) AdjustMyScrollbars();
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
}

void FbTreeViewMainWindow::OnSetFocus (wxFocusEvent &event)
{
	m_focused = true;
	event.Skip();
	Repaint();
}

void FbTreeViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
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
	bool ok = wxControl::SetFont(font);

	if ( m_header_win) m_header_win->SetFont(font);
	if ( m_main_win) m_main_win->SetFont(font);

	DoHeaderLayout();

	if (m_header_win) m_header_win->Refresh();

	return ok;
}

void FbTreeViewCtrl::AddColumn(size_t model_column, const wxString& text, int width, int flag, int fixed)
{
	if (m_header_win) m_header_win->AddColumn(FbTreeViewColumnInfo(model_column, text, width, flag, fixed));
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
