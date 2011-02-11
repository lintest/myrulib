/////////////////////////////////////////////////////////////////////////////
// Source:      src/generic/odcombo.cpp
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/wx.h>
#include <wx/combo.h>
#include "FbComboBox.h"

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
//  FbTreeViewHeaderWindow
//-----------------------------------------------------------------------------

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

wxBitmap FbSearchCombo::RenderButtonBitmap()
{
	wxSize sizeText = GetBestSize();
	int y  = sizeText.y - 4;
	int x = sizeText.y - 4;

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
    int penWidth = multiplier * 1;

    penWidth = penWidth * x / 20;

    wxBitmap bitmap( multiplier*x, multiplier*y );
	wxMemoryDC mem;

	wxColour bg = GetBackgroundColour();
    wxColour fg = mem.GetTextForeground();

	bg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

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

	penWidth = penWidth * 2;

    // draw handle
    int lineStart = radius + (radius-penWidth/2) * 707 / 1000; // 707 / 1000 = 0.707 = 1/sqrt(2);

    mem.SetPen( wxPen(fg) );
    mem.SetBrush( wxBrush(fg) );
    int handleCornerShift = penWidth * 707 / 1000 / 2; // 707 / 1000 = 0.707 = 1/sqrt(2);
	handleCornerShift = handleCornerShift > 0 ? handleCornerShift : 1;
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

    //===============================================================================
    // end drawing code
    //===============================================================================

    if ( multiplier != 1 )
    {
        wxImage image = bitmap.ConvertToImage();
        image.Rescale(x,y);
        bitmap = wxBitmap( image );
    }

    mem.SelectObject(wxNullBitmap);

    // Finalize transparency with a mask
    wxMask *mask = new wxMask(bitmap, bg);
    bitmap.SetMask(mask);

	bitmap = bitmap.GetSubBitmap(wxRect(0,0, y,y));

    return bitmap;
}

//-----------------------------------------------------------------------------
//  FbComboPopup
//-----------------------------------------------------------------------------

// time in milliseconds before partial completion buffer drops
#define fbODCB_PARTIAL_COMPLETION_TIME 1000

BEGIN_EVENT_TABLE(FbComboPopup, wxVListBox)
    EVT_MOTION(FbComboPopup::OnMouseMove)
    EVT_KEY_DOWN(FbComboPopup::OnKey)
    EVT_LEFT_UP(FbComboPopup::OnLeftClick)
END_EVENT_TABLE()


void FbComboPopup::Init()
{
    m_widestWidth = 0;
    m_widestItem = -1;
    m_widthsDirty = false;
    m_findWidest = false;
    m_itemHeight = 0;
    m_value = -1;
    m_itemHover = -1;
    m_clientDataItemsType = wxClientData_None;
    m_partialCompletionString = wxEmptyString;
}

bool FbComboPopup::Create(wxWindow* parent)
{
    if ( !wxVListBox::Create(parent,
                             wxID_ANY,
                             wxDefaultPosition,
                             wxDefaultSize,
                             wxBORDER_SIMPLE | wxLB_INT_HEIGHT | wxWANTS_CHARS) )
        return false;

    m_useFont = m_combo->GetFont();

    wxVListBox::SetItemCount(m_strings.GetCount());

    // TODO: Move this to SetFont
    m_itemHeight = GetCharHeight() + 0;

    return true;
}

FbComboPopup::~FbComboPopup()
{
    Clear();
}

bool FbComboPopup::LazyCreate()
{
    // NB: There is a bug with wxVListBox that can be avoided by creating
    //     it later (bug causes empty space to be shown if initial selection
    //     is at the end of a list longer than the control can show at once).
    return true;
}

// paint the control itself
void FbComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    if ( !(m_combo->GetWindowStyle() & wxODCB_STD_CONTROL_PAINT) )
    {
        int flags = wxODCB_PAINTING_CONTROL;

        if ( m_combo->ShouldDrawFocus() )
            flags |= wxODCB_PAINTING_SELECTED;

        OnDrawBg(dc, rect, m_value, flags);

        if ( m_value >= 0 )
        {
            OnDrawItem(dc,rect,m_value,flags);
            return;
        }
    }

    wxComboPopup::PaintComboControl(dc,rect);
}

void FbComboPopup::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
    // TODO: Maybe this code could be moved to wxVListBox::OnPaint?
    dc.SetFont(m_useFont);

    int flags = 0;

    // Set correct text colour for selected items
    if ( wxVListBox::GetSelection() == (int) n )
    {
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
        flags |= wxODCB_PAINTING_SELECTED;
    }
    else
    {
        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );
    }

    OnDrawItem(dc,rect,(int)n,flags);
}

wxCoord FbComboPopup::OnMeasureItem(size_t n) const
{
    FbComboBox* combo = (FbComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(FbComboBox)),
                  wxT("you must subclass FbComboPopup for drawing and measuring methods") );

    wxCoord h = combo->OnMeasureItem(n);
    if ( h < 0 )
        h = m_itemHeight;
    return h;
}

wxCoord FbComboPopup::OnMeasureItemWidth(size_t n) const
{
    FbComboBox* combo = (FbComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(FbComboBox)),
                  wxT("you must subclass FbComboPopup for drawing and measuring methods") );

    return combo->OnMeasureItemWidth(n);
}

void FbComboPopup::OnDrawBg( wxDC& dc,
                                     const wxRect& rect,
                                     int item,
                                     int flags ) const
{
    FbComboBox* combo = (FbComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(FbComboBox)),
                  wxT("you must subclass FbComboPopup for drawing and measuring methods") );

    if ( IsCurrent((size_t)item) && !(flags & wxODCB_PAINTING_CONTROL) )
        flags |= wxODCB_PAINTING_SELECTED;

    combo->OnDrawBackground(dc,rect,item,flags);
}

void FbComboPopup::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    OnDrawBg(dc,rect,(int)n,0);
}

// This is called from FbComboPopup::OnDrawItem, with text colour and font prepared
void FbComboPopup::OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
{
    FbComboBox* combo = (FbComboBox*) m_combo;

    wxASSERT_MSG( combo->IsKindOf(CLASSINFO(FbComboBox)),
                  wxT("you must subclass FbComboPopup for drawing and measuring methods") );

    combo->OnDrawItem(dc,rect,item,flags);
}

void FbComboPopup::DismissWithEvent()
{
    StopPartialCompletion();

    int selection = wxVListBox::GetSelection();

    Dismiss();

    wxString valStr;
    if ( selection != wxNOT_FOUND )
        valStr = m_strings[selection];
    else
        valStr = wxEmptyString;

    m_value = selection;

    if ( valStr != m_combo->GetValue() )
        m_combo->SetValueWithEvent(valStr);

    SendComboBoxEvent(selection);
}

void FbComboPopup::SendComboBoxEvent( int selection )
{
    wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED,m_combo->GetId());

    evt.SetEventObject(m_combo);

    evt.SetInt(selection);

    // Set client data, if any
    if ( selection >= 0 && (int)m_clientDatas.GetCount() > selection )
    {
        void* clientData = m_clientDatas[selection];
        if ( m_clientDataItemsType == wxClientData_Object )
            evt.SetClientObject((wxClientData*)clientData);
        else
            evt.SetClientData(clientData);
    }

    m_combo->GetEventHandler()->AddPendingEvent(evt);
}

// returns true if key was consumed
bool FbComboPopup::HandleKey( int keycode, bool saturate, wxChar unicode )
{
    const int itemCount = GetCount();

    // keys do nothing in the empty control and returning immediately avoids
    // using invalid indices below
    if ( !itemCount )
        return false;

    int value = m_value;
    int comboStyle = m_combo->GetWindowStyle();

    // this is the character equivalent of the code
    wxChar keychar = 0;
    if ( keycode < WXK_START )
    {
#if wxUSE_UNICODE
        if ( unicode > 0 )
        {
            if ( wxIsprint(unicode) )
                keychar = unicode;
        }
        else
#else
        wxUnusedVar(unicode);
#endif
        if ( wxIsprint(keycode) )
        {
            keychar = (wxChar) keycode;
        }
    }

    if ( keycode == WXK_DOWN || keycode == WXK_RIGHT )
    {
        value++;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_UP || keycode == WXK_LEFT )
    {
        value--;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_PAGEDOWN )
    {
        value+=10;
        StopPartialCompletion();
    }
    else if ( keycode == WXK_PAGEUP )
    {
        value-=10;
        StopPartialCompletion();
    }
    else if ( keychar && (comboStyle & wxCB_READONLY) )
    {
        // Try partial completion

        // find the new partial completion string
#if wxUSE_TIMER
        if (m_partialCompletionTimer.IsRunning())
            m_partialCompletionString+=wxString(keychar);
        else
#endif // wxUSE_TIMER
            m_partialCompletionString=wxString(keychar);

        // now search through the values to see if this is found
        int found = -1;
        unsigned int length=m_partialCompletionString.length();
        int i;
        for (i=0; i<itemCount; i++)
        {
            wxString item=GetString(i);
            if (( item.length() >= length) && (!  m_partialCompletionString.CmpNoCase(item.Left(length))))
            {
                found=i;
                break;
            }
        }

        if (found<0)
        {
            StopPartialCompletion();
            ::wxBell();
            return true; // to stop the first value being set
        }
        else
        {
            value=i;
#if wxUSE_TIMER
            m_partialCompletionTimer.Start(fbODCB_PARTIAL_COMPLETION_TIME, true);
#endif // wxUSE_TIMER
        }
    }
    else
        return false;

    if ( saturate )
    {
        if ( value >= itemCount )
            value = itemCount - 1;
        else if ( value < 0 )
            value = 0;
    }
    else
    {
        if ( value >= itemCount )
            value -= itemCount;
        else if ( value < 0 )
            value += itemCount;
    }

    if ( value == m_value )
        // Even if value was same, don't skip the event
        // (good for consistency)
        return true;

    m_value = value;

    if ( value >= 0 )
        m_combo->SetValue(m_strings[value]);

    SendComboBoxEvent(m_value);

    return true;
}

// stop partial completion
void FbComboPopup::StopPartialCompletion()
{
    m_partialCompletionString = wxEmptyString;
#if wxUSE_TIMER
    m_partialCompletionTimer.Stop();
#endif // wxUSE_TIMER
}

void FbComboPopup::OnComboDoubleClick()
{
    // Cycle on dclick (disable saturation to allow true cycling).
    if ( !::wxGetKeyState(WXK_SHIFT) )
        HandleKey(WXK_DOWN,false);
    else
        HandleKey(WXK_UP,false);
}

void FbComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    // Saturated key movement on
    if ( !HandleKey(event.GetKeyCode(),true,
#if wxUSE_UNICODE
        event.GetUnicodeKey()
#else
        0
#endif
        ) )
        event.Skip();
}

void FbComboPopup::OnPopup()
{
    // *must* set value after size is set (this is because of a vlbox bug)
    wxVListBox::SetSelection(m_value);
}

void FbComboPopup::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();

    // Move selection to cursor if it is inside the popup

    int y = event.GetPosition().y;
    int fromBottom = GetClientSize().y - y;

    // Since in any case we need to find out if the last item is only
    // partially visible, we might just as well replicate the HitTest
    // loop here.
    const size_t lineMax = GetVisibleEnd();
    for ( size_t line = GetVisibleBegin(); line < lineMax; line++ )
    {
        y -= OnGetLineHeight(line);
        if ( y < 0 )
        {
            // Only change selection if item is fully visible
            if ( (y + fromBottom) >= 0 )
            {
                wxVListBox::SetSelection((int)line);
                return;
            }
        }
    }
}

void FbComboPopup::OnLeftClick(wxMouseEvent& WXUNUSED(event))
{
    DismissWithEvent();
}

void FbComboPopup::OnKey(wxKeyEvent& event)
{
    // Hide popup if certain key or key combination was pressed
    if ( m_combo->IsKeyPopupToggle(event) )
    {
        StopPartialCompletion();
        Dismiss();
    }
    else if ( event.AltDown() )
    {
        // On both wxGTK and wxMSW, pressing Alt down seems to
        // completely freeze things in popup (ie. arrow keys and
        // enter won't work).
        return;
    }
    // Select item if ENTER is pressed
    else if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER )
    {
        DismissWithEvent();
    }
    else
    {
        int comboStyle = m_combo->GetWindowStyle();
        int keycode = event.GetKeyCode();
        // Process partial completion key codes here, but not the arrow keys as the base class will do that for us
        if ((comboStyle & wxCB_READONLY) &&
            (keycode >= WXK_SPACE) && (keycode <=255) && (keycode != WXK_DELETE) && wxIsprint(keycode))
        {
            OnComboKeyEvent(event);
            SetSelection(m_value); // ensure the highlight bar moves
        }
        else
            event.Skip();
    }
}

void FbComboPopup::Insert( const wxString& item, int pos )
{
    // Need to change selection?
    wxString strValue;
    if ( !(m_combo->GetWindowStyle() & wxCB_READONLY) &&
         m_combo->GetValue() == item )
    {
        m_value = pos;
    }

    m_strings.Insert(item,pos);
    m_widths.Insert(-1,pos);
    m_widthsDirty = true;

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()+1 );
}

int FbComboPopup::Append(const wxString& item)
{
    int pos = (int)m_strings.GetCount();

    if ( m_combo->GetWindowStyle() & wxCB_SORT )
    {
        // Find position
        // TODO: Could be optimized with binary search
        wxArrayString strings = m_strings;
        unsigned int i;

        for ( i=0; i<strings.GetCount(); i++ )
        {
            if ( item.CmpNoCase(strings.Item(i)) < 0 )
            {
                pos = (int)i;
                break;
            }
        }
    }

    Insert(item,pos);

    return pos;
}

void FbComboPopup::Clear()
{
    wxASSERT(m_combo);

    m_strings.Empty();
    m_widths.Empty();

    m_widestWidth = 0;
    m_widestItem = -1;

    ClearClientDatas();

    m_value = wxNOT_FOUND;

    if ( IsCreated() )
        wxVListBox::SetItemCount(0);
}

void FbComboPopup::ClearClientDatas()
{
    if ( m_clientDataItemsType == wxClientData_Object )
    {
        size_t i;
        for ( i=0; i<m_clientDatas.GetCount(); i++ )
            delete (wxClientData*) m_clientDatas[i];
    }

    m_clientDatas.Empty();
}

void FbComboPopup::SetItemClientData( unsigned int n,
                                              void* clientData,
                                              wxClientDataType clientDataItemsType )
{
    // It should be sufficient to update this variable only here
    m_clientDataItemsType = clientDataItemsType;

    m_clientDatas.SetCount(n+1,NULL);
    m_clientDatas[n] = clientData;

    ItemWidthChanged(n);
}

void* FbComboPopup::GetItemClientData(unsigned int n) const
{
    if ( m_clientDatas.GetCount() > n )
        return m_clientDatas[n];

    return NULL;
}

void FbComboPopup::Delete( unsigned int item )
{
    // Remove client data, if set
    if ( m_clientDatas.GetCount() )
    {
        if ( m_clientDataItemsType == wxClientData_Object )
            delete (wxClientData*) m_clientDatas[item];

        m_clientDatas.RemoveAt(item);
    }

    m_strings.RemoveAt(item);
    m_widths.RemoveAt(item);

    if ( (int)item == m_widestItem )
        m_findWidest = true;

    int sel = GetSelection();

    if ( IsCreated() )
        wxVListBox::SetItemCount( wxVListBox::GetItemCount()-1 );

    // Fix selection
    if ( (int)item < sel )
        SetSelection(sel-1);
    else if ( (int)item == sel )
        SetSelection(wxNOT_FOUND);
}

int FbComboPopup::FindString(const wxString& s, bool bCase) const
{
    return m_strings.Index(s, bCase);
}

unsigned int FbComboPopup::GetCount() const
{
    return m_strings.GetCount();
}

wxString FbComboPopup::GetString( int item ) const
{
    return m_strings[item];
}

void FbComboPopup::SetString( int item, const wxString& str )
{
    m_strings[item] = str;
    ItemWidthChanged(item);
}

wxString FbComboPopup::GetStringValue() const
{
    if ( m_value >= 0 )
        return m_strings[m_value];
    return wxEmptyString;
}

void FbComboPopup::SetSelection( int item )
{
    wxCHECK_RET( item == wxNOT_FOUND || ((unsigned int)item < GetCount()),
                 wxT("invalid index in FbComboPopup::SetSelection") );

    m_value = item;

    if ( IsCreated() )
        wxVListBox::SetSelection(item);
}

int FbComboPopup::GetSelection() const
{
    return m_value;
}

void FbComboPopup::SetStringValue( const wxString& value )
{
    int index = m_strings.Index(value);

    if ( index >= 0 && index < (int)wxVListBox::GetItemCount() )
    {
        m_value = index;
        wxVListBox::SetSelection(index);
    }
}

void FbComboPopup::CalcWidths()
{
    bool doFindWidest = m_findWidest;

    // Measure items with dirty width.
    if ( m_widthsDirty )
    {
        unsigned int i;
        unsigned int n = m_widths.GetCount();
        int dirtyHandled = 0;
        wxArrayInt& widths = m_widths;

        // I think using wxDC::GetTextExtent is faster than
        // wxWindow::GetTextExtent (assuming same dc is used
        // for all calls, as we do here).
        wxClientDC dc(m_combo);
        dc.SetFont(m_useFont);

        for ( i=0; i<n; i++ )
        {
            if ( widths[i] < 0 )
            {
                wxCoord x = OnMeasureItemWidth(i);

                if ( x < 0 )
                {
                    const wxString& text = m_strings[i];

                    // To make sure performance won't suck in extreme scenarios,
                    // we'll estimate length after some arbitrary number of items
                    // have been checked precily.
                    if ( dirtyHandled < 1024 )
                    {
                        wxCoord y;
                        dc.GetTextExtent(text, &x, &y, 0, 0);
                        x += 4;
                    }
                    else
                    {
                        x = text.length() * (dc.GetCharWidth()+1);
                    }
                }

                widths[i] = x;

                if ( x >= m_widestWidth )
                {
                    m_widestWidth = x;
                    m_widestItem = (int)i;
                }
                else if ( (int)i == m_widestItem )
                {
                    // Width of previously widest item has been decreased, so
                    // we'll have to check all to find current widest item.
                    doFindWidest = true;
                }

                dirtyHandled++;
            }
        }

        m_widthsDirty = false;
    }

    if ( doFindWidest )
    {
        unsigned int i;
        unsigned int n = m_widths.GetCount();

        int bestWidth = -1;
        int bestIndex = -1;

        for ( i=0; i<n; i++ )
        {
            int w = m_widths[i];
            if ( w > bestWidth )
            {
                bestIndex = (int)i;
                bestWidth = w;
            }
        }

        m_widestWidth = bestWidth;
        m_widestItem = bestIndex;

        m_findWidest = false;
    }
}

wxSize FbComboPopup::GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
{
    int height = 250;

    maxHeight -= 2;  // Must take borders into account

    if ( m_strings.GetCount() )
    {
        if ( prefHeight > 0 )
            height = prefHeight;

        if ( height > maxHeight )
            height = maxHeight;

        int totalHeight = GetTotalHeight(); // + 3;

#if defined(__WXMAC__)
        // Take borders into account, or there will be scrollbars even for one or two items.
        totalHeight += 2;
#endif
        if ( height >= totalHeight )
        {
            height = totalHeight;
        }
        else
        {
            // Adjust height to a multiple of the height of the first item
            // NB: Calculations that take variable height into account
            //     are unnecessary.
            int fih = GetLineHeight(0);
            height -= height % fih;
        }
    }
    else
        height = 50;

    CalcWidths();

    // Take scrollbar into account in width calculations
    int widestWidth = m_widestWidth + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    return wxSize(minWidth > widestWidth ? minWidth : widestWidth,
                  height+2);
}

//void FbComboPopup::Populate( int n, const wxString choices[] )
void FbComboPopup::Populate( const wxArrayString& choices )
{
    int i;

    int n = choices.GetCount();

    for ( i=0; i<n; i++ )
    {
        const wxString& item = choices.Item(i);
        m_strings.Add(item);
    }

    m_widths.SetCount(n,-1);
    m_widthsDirty = true;

    if ( IsCreated() )
        wxVListBox::SetItemCount(n);

    // Sort the initial choices
    if ( m_combo->GetWindowStyle() & wxCB_SORT )
        m_strings.Sort();

    // Find initial selection
    wxString strValue = m_combo->GetValue();
    if ( strValue.length() )
        m_value = m_strings.Index(strValue);
}

// ----------------------------------------------------------------------------
// FbComboBox
// ----------------------------------------------------------------------------


BEGIN_EVENT_TABLE(FbComboBox, wxComboCtrl)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS2(FbComboBox, wxComboCtrl, wxControlWithItems)

void FbComboBox::Init()
{
}

bool FbComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    return wxComboCtrl::Create(parent,id,value,pos,size,style,validator,name);
}

FbComboBox::FbComboBox(wxWindow *parent,
                                           wxWindowID id,
                                           const wxString& value,
                                           const wxPoint& pos,
                                           const wxSize& size,
                                           const wxArrayString& choices,
                                           long style,
                                           const wxValidator& validator,
                                           const wxString& name)
    : wxComboCtrl()
{
    Init();

    Create(parent,id,value,pos,size,choices,style, validator, name);
}

bool FbComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  const wxArrayString& choices,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    m_initChs = choices;
    //wxCArrayString chs(choices);

    //return Create(parent, id, value, pos, size, chs.GetCount(),
    //              chs.GetStrings(), style, validator, name);
    return Create(parent, id, value, pos, size, 0,
                  NULL, style, validator, name);
}

bool FbComboBox::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  int n,
                                  const wxString choices[],
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{

    if ( !Create(parent, id, value, pos, size, style,
                 validator, name) )
    {
        return false;
    }

    int i;
    for ( i=0; i<n; i++ )
        m_initChs.Add(choices[i]);

    return true;
}

FbComboBox::~FbComboBox()
{
    if ( m_popupInterface )
        GetVListBoxComboPopup()->ClearClientDatas();
}

void FbComboBox::DoSetPopupControl(wxComboPopup* popup)
{
    if ( !popup )
    {
        popup = new FbComboPopup();
    }

    wxComboCtrl::DoSetPopupControl(popup);

    wxASSERT(popup);

    // Add initial choices to the wxVListBox
    if ( !GetVListBoxComboPopup()->GetCount() )
    {
        GetVListBoxComboPopup()->Populate(m_initChs);
        m_initChs.Clear();
    }
}

// ----------------------------------------------------------------------------
// FbComboBox item manipulation methods
// ----------------------------------------------------------------------------

void FbComboBox::Clear()
{
    EnsurePopupControl();

    GetVListBoxComboPopup()->Clear();

    SetValue(wxEmptyString);
}

void FbComboBox::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n), _T("invalid index in FbComboBox::Delete") );

    if ( GetSelection() == (int) n )
        SetValue(wxEmptyString);

    GetVListBoxComboPopup()->Delete(n);
}

unsigned int FbComboBox::GetCount() const
{
    if ( !m_popupInterface )
        return m_initChs.GetCount();

    return GetVListBoxComboPopup()->GetCount();
}

wxString FbComboBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString, _T("invalid index in FbComboBox::GetString") );

    if ( !m_popupInterface )
        return m_initChs.Item(n);

    return GetVListBoxComboPopup()->GetString(n);
}

void FbComboBox::SetString(unsigned int n, const wxString& s)
{
    EnsurePopupControl();

    wxCHECK_RET( IsValid(n), _T("invalid index in FbComboBox::SetString") );

    GetVListBoxComboPopup()->SetString(n,s);
}

int FbComboBox::FindString(const wxString& s, bool bCase) const
{
    if ( !m_popupInterface )
        return m_initChs.Index(s, bCase);

    return GetVListBoxComboPopup()->FindString(s, bCase);
}

void FbComboBox::Select(int n)
{
    EnsurePopupControl();

    wxCHECK_RET( (n == wxNOT_FOUND) || IsValid(n), _T("invalid index in FbComboBox::Select") );

    GetVListBoxComboPopup()->SetSelection(n);

    wxString str;
    if ( n >= 0 )
        str = GetVListBoxComboPopup()->GetString(n);

    // Refresh text portion in control
    if ( m_text )
        m_text->SetValue( str );
    else
        m_valueString = str;

    Refresh();
}

int FbComboBox::GetSelection() const
{
    if ( !m_popupInterface )
        return m_initChs.Index(m_valueString);

    return GetVListBoxComboPopup()->GetSelection();
}

int FbComboBox::DoAppend(const wxString& item)
{
    EnsurePopupControl();
    wxASSERT(m_popupInterface);

    return GetVListBoxComboPopup()->Append(item);
}

int FbComboBox::DoInsert(const wxString& item, unsigned int pos)
{
    EnsurePopupControl();

    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG(IsValidInsert(pos), -1, wxT("invalid index"));

    GetVListBoxComboPopup()->Insert(item,pos);

    return pos;
}

void FbComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    EnsurePopupControl();

    GetVListBoxComboPopup()->SetItemClientData(n,clientData,m_clientDataItemsType);
}

void* FbComboBox::DoGetItemClientData(unsigned int n) const
{
    if ( !m_popupInterface )
        return NULL;

    return GetVListBoxComboPopup()->GetItemClientData(n);
}

void FbComboBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    DoSetItemClientData(n, (void*) clientData);
}

wxClientData* FbComboBox::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData*) DoGetItemClientData(n);
}

// ----------------------------------------------------------------------------
// FbComboBox item drawing and measuring default implementations
// ----------------------------------------------------------------------------

void FbComboBox::OnDrawItem( wxDC& dc,
                                       const wxRect& rect,
                                       int item,
                                       int flags ) const
{
    if ( flags & wxODCB_PAINTING_CONTROL )
    {
        dc.DrawText( GetValue(),
                     rect.x + GetTextIndent(),
                     (rect.height-dc.GetCharHeight())/2 + rect.y );
    }
    else
    {
        dc.DrawText( GetVListBoxComboPopup()->GetString(item), rect.x + 2, rect.y );
    }
}

wxCoord FbComboBox::OnMeasureItem( size_t WXUNUSED(item) ) const
{
    return -1;
}

wxCoord FbComboBox::OnMeasureItemWidth( size_t WXUNUSED(item) ) const
{
    return -1;
}

void FbComboBox::OnDrawBackground(wxDC& dc,
                                            const wxRect& rect,
                                            int WXUNUSED(item),
                                            int flags) const
{
    // We need only to explicitly draw background for items
    // that should have selected background. Also, call PrepareBackground
    // always when painting the control so that clipping is done properly.

    if ( (flags & wxODCB_PAINTING_SELECTED) ||
         ((flags & wxODCB_PAINTING_CONTROL) && HasFlag(wxCB_READONLY)) )
    {
        int bgFlags = wxCONTROL_SELECTED;

        if ( !(flags & wxODCB_PAINTING_CONTROL) )
            bgFlags |= wxCONTROL_ISSUBMENU;

        PrepareBackground(dc, rect, bgFlags);
    }
}

