///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/srchctlg.cpp
// Purpose:     implements wxSearchCtrl as a composite control
// Author:      Vince Harron
// Created:     2006-02-19
// RCS-ID:      $Id: srchctlg.cpp 47962 2007-08-08 12:38:13Z JS $
// Copyright:   Vince Harron
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "FbSearchCtrl.h"


#define WXMAX(a,b) ((a)>(b)?(a):(b))

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the search/cancel buttons
static const wxCoord MARGIN = 2;

// border around all controls to compensate for wxSIMPLE_BORDER
#if defined(__WXMSW__)
static const wxCoord BORDER = 0;
static const wxCoord ICON_MARGIN = 2;
static const wxCoord ICON_OFFSET = 2;
#else
static const wxCoord BORDER = 2;
static const wxCoord ICON_MARGIN = 0;
static const wxCoord ICON_OFFSET = 0;
#endif

// ----------------------------------------------------------------------------
// TODO: These functions or something like them should probably be made
// public.  There are similar functions in src/aui/dockart.cpp...

static double wxBlendColour(double fg, double bg, double alpha)
{
    double result = bg + (alpha * (fg - bg));
    if (result < 0.0)
        result = 0.0;
    if (result > 255)
        result = 255;
    return result;
}

static wxColor wxStepColour(const wxColor& c, int ialpha)
{
    if (ialpha == 100)
        return c;

    double r = c.Red(), g = c.Green(), b = c.Blue();
    double bg;

    // ialpha is 0..200 where 0 is completely black
    // and 200 is completely white and 100 is the same
    // convert that to normal alpha 0.0 - 1.0
    ialpha = wxMin(ialpha, 200);
    ialpha = wxMax(ialpha, 0);
    double alpha = ((double)(ialpha - 100.0))/100.0;

    if (ialpha > 100)
    {
        // blend with white
        bg = 255.0;
        alpha = 1.0 - alpha;  // 0 = transparent fg; 1 = opaque fg
    }
     else
    {
        // blend with black
        bg = 0.0;
        alpha = 1.0 + alpha;  // 0 = transparent fg; 1 = opaque fg
    }

    r = wxBlendColour(r, bg, alpha);
    g = wxBlendColour(g, bg, alpha);
    b = wxBlendColour(b, bg, alpha);

    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

#define LIGHT_STEP 160

// icons are rendered at 3-8 times larger than necessary and downscaled for
// antialiasing
static int GetMultiplier()
{
#ifdef __WXWINCE__
    // speed up bitmap generation by using a small bitmap
    return 3;
#else
    int depth = ::wxDisplayDepth();

    if  ( depth >= 24 )
    {
        return 8;
    }
    return 6;
#endif
}

#ifdef FB_SEARCH_COMBO_CTRL

#include <wx/image.h>

IMPLEMENT_CLASS(FbSearchCtrl, wxOwnerDrawnComboBox)

BEGIN_EVENT_TABLE(FbSearchCtrl, wxOwnerDrawnComboBox)
    EVT_MENU(wxID_SELECTALL, FbSearchCtrl::OnSelectAll)
END_EVENT_TABLE()

FbSearchCtrl::FbSearchCtrl()
{
    Init();
}

FbSearchCtrl::FbSearchCtrl(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    Init();
    Create(parent, id, value, pos, size, style, validator, name);
}

void FbSearchCtrl::Init()
{
}

bool FbSearchCtrl::Create(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
{
    if (!wxOwnerDrawnComboBox::Create(parent, id, value, pos, size, style, validator, name)) return false;
    wxSize sizeText = GetBestSize();
    int bitmapHeight = sizeText.y - 4 * ICON_MARGIN;
    int bitmapWidth  = sizeText.y * 20 / 14;
	wxBitmap bitmap = RenderCancelBitmap(bitmapWidth, bitmapHeight);
    SetButtonBitmaps(bitmap, false);
    return true;
}

FbSearchCtrl::~FbSearchCtrl()
{
}

wxBitmap FbSearchCtrl::RenderCancelBitmap( int x, int y )
{
    wxColour bg = GetBackgroundColour();
    wxColour fg = wxStepColour(GetForegroundColour(), LIGHT_STEP);

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // total size 14x14
    // force 1:1 ratio
    if ( x > y )
    {
        // x is too big
        x = y;
    }
    else
    {
        // y is too big
        y = x;
    }

    // 14x14 circle
    // cross line starts (4,4)-(10,10)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();

    int penWidth = multiplier * x / 14;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int radius = multiplier*x/2;
    mem.DrawCircle(radius,radius,radius);

    // draw cross
    int lineStartBase = 4 * x / 14;
    int lineLength = x - 2*lineStartBase;

    mem.SetPen( wxPen(bg) );
    mem.SetBrush( wxBrush(bg) );
    int handleCornerShift = penWidth/2;
    handleCornerShift = WXMAX( handleCornerShift, 1 );
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,multiplier*lineStartBase,multiplier*lineStartBase);
    wxPoint handlePolygon2[] =
    {
        wxPoint(+handleCornerShift,+handleCornerShift),
        wxPoint(-handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,-multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,-multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon2),handlePolygon2,multiplier*lineStartBase,multiplier*(x-lineStartBase));

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale(x,y);
        bitmap = wxBitmap( image );
    }

    return bitmap;
}

#else

#if wxABI_VERSION >= 20900

#else

#include "wx/button.h"
#include "wx/dcclient.h"
#include "wx/menu.h"
#include "wx/dcmemory.h"

// ----------------------------------------------------------------------------
// FbSearchTextCtrl: text control used by search control
// ----------------------------------------------------------------------------

class FbSearchTextCtrl : public wxTextCtrl
{
public:
    FbSearchTextCtrl(FbSearchCtrl *search, const wxString& value, int style)
        : wxTextCtrl(search, wxID_ANY, value, wxDefaultPosition, wxDefaultSize,
                     style | wxNO_BORDER)
    {
        m_search = search;
        m_defaultFG = GetForegroundColour();

        // remove the default minsize, the searchctrl will have one instead
        SetSizeHints(wxDefaultCoord,wxDefaultCoord);
    }

    void SetDescriptiveText(const wxString& text)
    {
        if ( GetValue() == m_descriptiveText )
        {
            ChangeValue(wxEmptyString);
        }

        m_descriptiveText = text;
    }

    wxString GetDescriptiveText() const
    {
        return m_descriptiveText;
    }

protected:
    void OnText(wxCommandEvent& eventText)
    {
    	m_search->ShowCancelButton(!m_search->GetValue().IsEmpty());
    }

    void OnTextEnter(wxCommandEvent& event)
    {
    	wxCommandEvent e(wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, m_search->GetId());
        event.SetEventObject(m_search);
        m_search->GetEventHandler()->ProcessEvent(e);
    }

    void OnIdle(wxIdleEvent& WXUNUSED(event))
    {
        if ( IsEmpty() && !(wxWindow::FindFocus() == this) )
        {
            ChangeValue(m_descriptiveText);
            SetInsertionPoint(0);
            SetForegroundColour(wxStepColour(m_defaultFG, LIGHT_STEP));
        }
    }

    void OnFocus(wxFocusEvent& event)
    {
        event.Skip();
        if ( GetValue() == m_descriptiveText )
        {
            ChangeValue(wxEmptyString);
            SetForegroundColour(m_defaultFG);
        }
    }

    void OnSelectAll(wxCommandEvent& event)
    {
    	SelectAll();
    }

private:
    FbSearchCtrl* m_search;
    wxString      m_descriptiveText;
    wxColour      m_defaultFG;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FbSearchTextCtrl, wxTextCtrl)
    EVT_MENU(wxID_SELECTALL, FbSearchTextCtrl::OnSelectAll)
    EVT_TEXT(wxID_ANY, FbSearchTextCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, FbSearchTextCtrl::OnTextEnter)
    EVT_IDLE(FbSearchTextCtrl::OnIdle)
    EVT_SET_FOCUS(FbSearchTextCtrl::OnFocus)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// FbSearchButton: search button used by search control
// ----------------------------------------------------------------------------

class FbSearchButton : public wxControl
{
public:
    FbSearchButton(FbSearchCtrl *search, int eventType, const wxBitmap& bmp)
        : wxControl(search, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
          m_search(search),
          m_eventType(eventType),
          m_bmp(bmp)
    { }

    void SetBitmapLabel(const wxBitmap& label) { m_bmp = label; }


protected:
    wxSize DoGetBestSize() const
    {
        return wxSize(m_bmp.GetWidth(), m_bmp.GetHeight());
    }

    void OnLeftUp(wxMouseEvent&)
    {
        wxCommandEvent event(m_eventType, m_search->GetId());
        event.SetEventObject(m_search);

        GetEventHandler()->ProcessEvent(event);

        m_search->SetFocus();
    }

    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        dc.DrawBitmap(m_bmp, 0,0, true);
    }


private:
    FbSearchCtrl *m_search;
    wxEventType   m_eventType;
    wxBitmap      m_bmp;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FbSearchButton, wxControl)
    EVT_LEFT_UP(FbSearchButton::OnLeftUp)
    EVT_PAINT(FbSearchButton::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(FbSearchCtrl, FbSearchCtrlBase)
    EVT_SEARCHCTRL_SEARCH_BTN(wxID_ANY, FbSearchCtrl::OnSearchButton)
    EVT_SEARCHCTRL_CANCEL_BTN(wxID_ANY, FbSearchCtrl::OnCancelButton)
    EVT_SET_FOCUS(FbSearchCtrl::OnSetFocus)
    EVT_SIZE(FbSearchCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(FbSearchCtrl, FbSearchCtrlBase)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// FbSearchCtrl creation
// ----------------------------------------------------------------------------

// creation
// --------

FbSearchCtrl::FbSearchCtrl()
{
    Init();
}

FbSearchCtrl::FbSearchCtrl(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Init();

    Create(parent, id, value, pos, size, style, validator, name);
}

void FbSearchCtrl::Init()
{
    m_text = NULL;
    m_searchButton = NULL;
    m_cancelButton = NULL;

    m_searchButtonVisible = true;
    m_cancelButtonVisible = false;

    m_searchBitmapUser = false;
    m_cancelBitmapUser = false;
}

bool FbSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
	int borderStyle = wxBORDER_SIMPLE;

#if defined(__WXMSW__)
    borderStyle = GetThemedBorderStyle();
    if (borderStyle == wxBORDER_SUNKEN)
        borderStyle = wxBORDER_SIMPLE;
#elif defined(__WXGTK__)
    borderStyle = wxBORDER_SUNKEN;
#endif

    if ( !wxTextCtrlBase::Create(parent, id, pos, size, borderStyle | (style & ~wxBORDER_MASK), validator, name) )
    {
        return false;
    }

    m_text = new FbSearchTextCtrl(this, value, style & ~wxBORDER_MASK);
    m_text->SetDescriptiveText(_("Search"));

    m_searchButton = new FbSearchButton(this,wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN,m_searchBitmap);
    m_cancelButton = new FbSearchButton(this,wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN,m_cancelBitmap);

    SetForegroundColour( m_text->GetForegroundColour() );
    m_searchButton->SetForegroundColour( m_text->GetForegroundColour() );
    m_cancelButton->SetForegroundColour( m_text->GetForegroundColour() );

    SetBackgroundColour( m_text->GetBackgroundColour() );
    m_searchButton->SetBackgroundColour( m_text->GetBackgroundColour() );
    m_cancelButton->SetBackgroundColour( m_text->GetBackgroundColour() );

    RecalcBitmaps();

    SetInitialSize(size);
    Move(pos);
    return true;
}

FbSearchCtrl::~FbSearchCtrl()
{
    delete m_text;
    delete m_searchButton;
    delete m_cancelButton;
}

void FbSearchCtrl::ShowSearchButton( bool show )
{
    if ( m_searchButtonVisible == show )
    {
        // no change
        return;
    }
    m_searchButtonVisible = show;
    if ( m_searchButtonVisible )
    {
        RecalcBitmaps();
    }

    wxRect rect = GetRect();
    LayoutControls(0, 0, rect.GetWidth(), rect.GetHeight());
}

bool FbSearchCtrl::IsSearchButtonVisible() const
{
    return m_searchButtonVisible;
}


void FbSearchCtrl::ShowCancelButton( bool show )
{
    if ( m_cancelButtonVisible == show )
    {
        // no change
        return;
    }
    m_cancelButtonVisible = show;

    wxRect rect = GetRect();
    LayoutControls(0, 0, rect.GetWidth(), rect.GetHeight());
}

bool FbSearchCtrl::IsCancelButtonVisible() const
{
    return m_cancelButtonVisible;
}

void FbSearchCtrl::SetDescriptiveText(const wxString& text)
{
    m_text->SetDescriptiveText(text);
}

wxString FbSearchCtrl::GetDescriptiveText() const
{
    return m_text->GetDescriptiveText();
}

// ----------------------------------------------------------------------------
// geometry
// ----------------------------------------------------------------------------

wxSize FbSearchCtrl::DoGetBestSize() const
{
    wxSize sizeText = m_text->GetBestSize();
    wxSize sizeSearch(0,0);
    wxSize sizeCancel(0,0);
    int searchMargin = 0;
    int cancelMargin = 0;
    if ( m_searchButtonVisible )
    {
        sizeSearch = m_searchButton->GetBestSize();
        searchMargin = MARGIN;
    }
    if ( m_cancelButtonVisible )
    {
        sizeCancel = m_cancelButton->GetBestSize();
        cancelMargin = MARGIN;
    }

    int horizontalBorder = 1 + ( sizeText.y - sizeText.y * 14 / 21 ) / 2;

    // buttons are square and equal to the height of the text control
    int height = sizeText.y;
    return wxSize(sizeSearch.x + searchMargin + sizeText.x + cancelMargin + sizeCancel.x + 2*horizontalBorder,
                  height + 2*BORDER);
}

void FbSearchCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    FbSearchCtrlBase::DoMoveWindow(x, y, width, height);

    LayoutControls(0, 0, width, height);
}

void FbSearchCtrl::LayoutControls(int x, int y, int width, int height)
{
    if ( !m_text )
        return;

    wxSize sizeText = m_text->GetBestSize();
    // make room for the search menu & clear button
    int horizontalBorder = ( sizeText.y - sizeText.y * 14 / 21 ) / 2;
    x += horizontalBorder;
    y += BORDER;
    width -= horizontalBorder*2;
    height -= BORDER*2;

    wxSize sizeSearch(0,0);
    wxSize sizeCancel(0,0);
    int searchMargin = 0;
    int cancelMargin = 0;
    if ( m_searchButtonVisible )
    {
        sizeSearch = m_searchButton->GetBestSize();
        searchMargin = MARGIN;
    }
    if ( m_cancelButtonVisible )
    {
        sizeCancel = m_cancelButton->GetBestSize();
        cancelMargin = MARGIN;
    }
    m_searchButton->Show( m_searchButtonVisible );
    m_cancelButton->Show( m_cancelButtonVisible );

    if ( sizeSearch.x + sizeCancel.x > width )
    {
        sizeSearch.x = width/2;
        sizeCancel.x = width/2;
        searchMargin = 0;
        cancelMargin = 0;
    }
    wxCoord textWidth = width - sizeSearch.x - sizeCancel.x - searchMargin - cancelMargin - 1;

    // position the subcontrols inside the client area

    m_searchButton->SetSize(x, y + ICON_OFFSET - 1, sizeSearch.x, height);
    m_text->SetSize( x + sizeSearch.x + searchMargin,
                     y + ICON_OFFSET - BORDER,
                     textWidth,
                     height);
    m_cancelButton->SetSize(x + sizeSearch.x + searchMargin + textWidth + cancelMargin,
                            y + ICON_OFFSET - 1, sizeCancel.x, height);
}


// accessors
// ---------

wxString FbSearchCtrl::GetValue() const
{
    wxString value = m_text->GetValue();
    if (value == m_text->GetDescriptiveText())
        return wxEmptyString;
    else
        return value;
}
void FbSearchCtrl::SetValue(const wxString& value)
{
    m_text->SetValue(value);
}

wxString FbSearchCtrl::GetRange(long from, long to) const
{
    return m_text->GetRange(from, to);
}

int FbSearchCtrl::GetLineLength(long lineNo) const
{
    return m_text->GetLineLength(lineNo);
}
wxString FbSearchCtrl::GetLineText(long lineNo) const
{
    return m_text->GetLineText(lineNo);
}
int FbSearchCtrl::GetNumberOfLines() const
{
    return m_text->GetNumberOfLines();
}

bool FbSearchCtrl::IsModified() const
{
    return m_text->IsModified();
}
bool FbSearchCtrl::IsEditable() const
{
    return m_text->IsEditable();
}

// more readable flag testing methods
bool FbSearchCtrl::IsSingleLine() const
{
    return m_text->IsSingleLine();
}
bool FbSearchCtrl::IsMultiLine() const
{
    return m_text->IsMultiLine();
}

// If the return values from and to are the same, there is no selection.
void FbSearchCtrl::GetSelection(long* from, long* to) const
{
    m_text->GetSelection(from, to);
}

wxString FbSearchCtrl::GetStringSelection() const
{
    return m_text->GetStringSelection();
}

// operations
// ----------

// editing
void FbSearchCtrl::Clear()
{
    m_text->Clear();
	ShowCancelButton(false);
}

void FbSearchCtrl::Replace(long from, long to, const wxString& value)
{
    m_text->Replace(from, to, value);
}

void FbSearchCtrl::Remove(long from, long to)
{
    m_text->Remove(from, to);
}

// load/save the controls contents from/to the file
bool FbSearchCtrl::LoadFile(const wxString& file)
{
    return m_text->LoadFile(file);
}
bool FbSearchCtrl::SaveFile(const wxString& file)
{
    return m_text->SaveFile(file);
}

// sets/clears the dirty flag
void FbSearchCtrl::MarkDirty()
{
    m_text->MarkDirty();
}
void FbSearchCtrl::DiscardEdits()
{
    m_text->DiscardEdits();
}

// set the max number of characters which may be entered in a single line
// text control
void FbSearchCtrl::SetMaxLength(unsigned long len)
{
    m_text->SetMaxLength(len);
}

// writing text inserts it at the current position, appending always
// inserts it at the end
void FbSearchCtrl::WriteText(const wxString& text)
{
    m_text->WriteText(text);
}
void FbSearchCtrl::AppendText(const wxString& text)
{
    m_text->AppendText(text);
}

// insert the character which would have resulted from this key event,
// return true if anything has been inserted
bool FbSearchCtrl::EmulateKeyPress(const wxKeyEvent& event)
{
    return m_text->EmulateKeyPress(event);
}

// text control under some platforms supports the text styles: these
// methods allow to apply the given text style to the given selection or to
// set/get the style which will be used for all appended text
bool FbSearchCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return m_text->SetStyle(start, end, style);
}
bool FbSearchCtrl::GetStyle(long position, wxTextAttr& style)
{
    return m_text->GetStyle(position, style);
}
bool FbSearchCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    return m_text->SetDefaultStyle(style);
}
const wxTextAttr& FbSearchCtrl::GetDefaultStyle() const
{
    return m_text->GetDefaultStyle();
}

// translate between the position (which is just an index in the text ctrl
// considering all its contents as a single strings) and (x, y) coordinates
// which represent column and line.
long FbSearchCtrl::XYToPosition(long x, long y) const
{
    return m_text->XYToPosition(x, y);
}
bool FbSearchCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return m_text->PositionToXY(pos, x, y);
}

void FbSearchCtrl::ShowPosition(long pos)
{
    m_text->ShowPosition(pos);
}

// find the character at position given in pixels
//
// NB: pt is in device coords (not adjusted for the client area origin nor
//     scrolling)
wxTextCtrlHitTestResult FbSearchCtrl::HitTest(const wxPoint& pt, long *pos) const
{
    return m_text->HitTest(pt, pos);
}
wxTextCtrlHitTestResult FbSearchCtrl::HitTest(const wxPoint& pt,
                                        wxTextCoord *col,
                                        wxTextCoord *row) const
{
    return m_text->HitTest(pt, col, row);
}

// Clipboard operations
void FbSearchCtrl::Copy()
{
    m_text->Copy();
}
void FbSearchCtrl::Cut()
{
    m_text->Cut();
}
void FbSearchCtrl::Paste()
{
    m_text->Paste();
}

bool FbSearchCtrl::CanCopy() const
{
    return m_text->CanCopy();
}
bool FbSearchCtrl::CanCut() const
{
    return m_text->CanCut();
}
bool FbSearchCtrl::CanPaste() const
{
    return m_text->CanPaste();
}

// Undo/redo
void FbSearchCtrl::Undo()
{
    m_text->Undo();
}
void FbSearchCtrl::Redo()
{
    m_text->Redo();
}

bool FbSearchCtrl::CanUndo() const
{
    return m_text->CanUndo();
}
bool FbSearchCtrl::CanRedo() const
{
    return m_text->CanRedo();
}

// Insertion point
void FbSearchCtrl::SetInsertionPoint(long pos)
{
    m_text->SetInsertionPoint(pos);
}
void FbSearchCtrl::SetInsertionPointEnd()
{
    m_text->SetInsertionPointEnd();
}
long FbSearchCtrl::GetInsertionPoint() const
{
    return m_text->GetInsertionPoint();
}
wxTextPos FbSearchCtrl::GetLastPosition() const
{
    return m_text->GetLastPosition();
}

void FbSearchCtrl::SetSelection(long from, long to)
{
    m_text->SetSelection(from, to);
}
void FbSearchCtrl::SelectAll()
{
    m_text->SelectAll();
}

void FbSearchCtrl::SetEditable(bool editable)
{
    m_text->SetEditable(editable);
}

bool FbSearchCtrl::SetFont(const wxFont& font)
{
    bool result = FbSearchCtrlBase::SetFont(font);
    if ( result && m_text )
    {
        result = m_text->SetFont(font);
    }
    RecalcBitmaps();
    return result;
}

// search control generic only
void FbSearchCtrl::SetSearchBitmap( const wxBitmap& bitmap )
{
    m_searchBitmap = bitmap;
    m_searchBitmapUser = bitmap.Ok();
    if ( m_searchBitmapUser )
    {
        if ( m_searchButton )
        {
            m_searchButton->SetBitmapLabel( m_searchBitmap );
        }
    }
    else
    {
        // the user bitmap was just cleared, generate one
        RecalcBitmaps();
    }
}

void FbSearchCtrl::SetCancelBitmap( const wxBitmap& bitmap )
{
    m_cancelBitmap = bitmap;
    m_cancelBitmapUser = bitmap.Ok();
    if ( m_cancelBitmapUser )
    {
        if ( m_cancelButton )
        {
            m_cancelButton->SetBitmapLabel( m_cancelBitmap );
        }
    }
    else
    {
        // the user bitmap was just cleared, generate one
        RecalcBitmaps();
    }
}

void FbSearchCtrl::DoSetValue(const wxString& value, int flags)
{
    m_text->ChangeValue( value );
    if ( flags & SetValue_SendEvent )
        SendTextUpdatedEvent();
}

// do the window-specific processing after processing the update event
void FbSearchCtrl::DoUpdateWindowUI(wxUpdateUIEvent& event)
{
    FbSearchCtrlBase::DoUpdateWindowUI(event);
}

bool FbSearchCtrl::ShouldInheritColours() const
{
    return true;
}

wxBitmap FbSearchCtrl::RenderSearchBitmap( int x, int y, bool renderDrop )
{
    wxColour bg = GetBackgroundColour();
    wxColour fg = wxStepColour(GetForegroundColour(), LIGHT_STEP-20);

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // force width:height ratio
    if ( 14*x > y*20 )
    {
        // x is too big
        x = y*20/14;
    }
    else
    {
        // y is too big
        y = x*14/20;
    }

    // glass 11x11, top left corner
    // handle (9,9)-(13,13)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();
    int penWidth = multiplier * 2;

    penWidth = penWidth * x / 20;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int glassBase = 5 * x / 20;
    int glassFactor = 2*glassBase + 1;
    int radius = multiplier*glassFactor/2;
    mem.DrawCircle(radius,radius,radius);
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawCircle(radius,radius,radius-penWidth);

    // draw handle
    int lineStart = radius + (radius-penWidth/2) * 707 / 1000; // 707 / 1000 = 0.707 = 1/sqrt(2);

    mem.SetPen( wxPen(fg) );
    mem.SetBrush( wxBrush(fg) );
    int handleCornerShift = penWidth * 707 / 1000 / 2; // 707 / 1000 = 0.707 = 1/sqrt(2);
    handleCornerShift = WXMAX( handleCornerShift, 1 );
    int handleBase = 4 * x / 20;
    int handleLength = 2*handleBase+1;
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*handleLength/2+handleCornerShift,multiplier*handleLength/2-handleCornerShift),
        wxPoint(multiplier*handleLength/2-handleCornerShift,multiplier*handleLength/2+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,lineStart,lineStart);

    // draw drop triangle
    int triangleX = 13 * x / 20;
    int triangleY = 5 * x / 20;
    int triangleBase = 3 * x / 20;
    int triangleFactor = triangleBase*2+1;
    if ( renderDrop )
    {
        wxPoint dropPolygon[] =
        {
            wxPoint(multiplier*0,multiplier*0), // triangle left
            wxPoint(multiplier*triangleFactor-1,multiplier*0), // triangle right
            wxPoint(multiplier*triangleFactor/2,multiplier*triangleFactor/2), // triangle bottom
        };
        mem.DrawPolygon(WXSIZEOF(dropPolygon),dropPolygon,multiplier*triangleX,multiplier*triangleY);
    }
    mem.SelectObject(wxNullBitmap);

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale(x,y);
        bitmap = wxBitmap( image );
    }
    if ( !renderDrop )
    {
        // Trim the edge where the arrow would have gone
        bitmap = bitmap.GetSubBitmap(wxRect(0,0, y,y));
    }

    return bitmap;
}

wxBitmap FbSearchCtrl::RenderCancelBitmap( int x, int y )
{
    wxColour bg = GetBackgroundColour();
    wxColour fg = wxStepColour(GetForegroundColour(), LIGHT_STEP);

    //===============================================================================
    // begin drawing code
    //===============================================================================
    // image stats

    // total size 14x14
    // force 1:1 ratio
    if ( x > y )
    {
        // x is too big
        x = y;
    }
    else
    {
        // y is too big
        y = x;
    }

    // 14x14 circle
    // cross line starts (4,4)-(10,10)
    // drop (13,16)-(19,6)-(16,9)

    int multiplier = GetMultiplier();

    int penWidth = multiplier * x / 14;

    wxBitmap bitmap( multiplier*x, multiplier*y );
    wxMemoryDC mem;
    mem.SelectObject(bitmap);

    // clear background
    mem.SetBrush( wxBrush(bg) );
    mem.SetPen( wxPen(bg) );
    mem.DrawRectangle(0,0,bitmap.GetWidth(),bitmap.GetHeight());

    // draw drop glass
    mem.SetBrush( wxBrush(fg) );
    mem.SetPen( wxPen(fg) );
    int radius = multiplier*x/2;
    mem.DrawCircle(radius,radius,radius);

    // draw cross
    int lineStartBase = 4 * x / 14;
    int lineLength = x - 2*lineStartBase;

    mem.SetPen( wxPen(bg) );
    mem.SetBrush( wxBrush(bg) );
    int handleCornerShift = penWidth/2;
    handleCornerShift = WXMAX( handleCornerShift, 1 );
    wxPoint handlePolygon[] =
    {
        wxPoint(-handleCornerShift,+handleCornerShift),
        wxPoint(+handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon),handlePolygon,multiplier*lineStartBase,multiplier*lineStartBase);
    wxPoint handlePolygon2[] =
    {
        wxPoint(+handleCornerShift,+handleCornerShift),
        wxPoint(-handleCornerShift,-handleCornerShift),
        wxPoint(multiplier*lineLength-handleCornerShift,-multiplier*lineLength-handleCornerShift),
        wxPoint(multiplier*lineLength+handleCornerShift,-multiplier*lineLength+handleCornerShift),
    };
    mem.DrawPolygon(WXSIZEOF(handlePolygon2),handlePolygon2,multiplier*lineStartBase,multiplier*(x-lineStartBase));

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale(x,y);
        bitmap = wxBitmap( image );
    }

    return bitmap;
}

void FbSearchCtrl::RecalcBitmaps()
{
    if ( !m_text )
    {
        return;
    }
    wxSize sizeText = m_text->GetBestSize();

    int bitmapHeight = sizeText.y - 2 * ICON_MARGIN;
    int bitmapWidth  = sizeText.y * 20 / 14;

    if ( !m_searchBitmapUser )
    {
        if (
            !m_searchBitmap.Ok() ||
            m_searchBitmap.GetHeight() != bitmapHeight ||
            m_searchBitmap.GetWidth() != bitmapWidth
            )
        {
            m_searchBitmap = RenderSearchBitmap(bitmapWidth,bitmapHeight,false);
            m_searchButton->SetBitmapLabel(m_searchBitmap);
        }
        // else this bitmap was set by user, don't alter
    }

    if ( !m_cancelBitmapUser )
    {
        if (
            !m_cancelBitmap.Ok() ||
            m_cancelBitmap.GetHeight() != bitmapHeight ||
            m_cancelBitmap.GetWidth() != bitmapHeight
            )
        {
            m_cancelBitmap = RenderCancelBitmap(bitmapHeight-BORDER-1,bitmapHeight-BORDER-1); // square
            m_cancelButton->SetBitmapLabel(m_cancelBitmap);
        }
        // else this bitmap was set by user, don't alter
    }
}

void FbSearchCtrl::OnSearchButton( wxCommandEvent& event )
{
    event.Skip();
}

void FbSearchCtrl::OnCancelButton( wxCommandEvent& event )
{
	SetValue(wxEmptyString);
	ShowCancelButton(false);
}


void FbSearchCtrl::OnSetFocus( wxFocusEvent& /*event*/ )
{
    if ( m_text )
    {
        m_text->SetFocus();
    }
}

void FbSearchCtrl::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    int width, height;
    GetSize(&width, &height);
    LayoutControls(0, 0, width, height);
}

#endif // wxABI_VERSION

#endif // FB_SEARCH_COMBO_CTRL
