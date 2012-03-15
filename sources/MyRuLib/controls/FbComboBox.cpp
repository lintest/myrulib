/////////////////////////////////////////////////////////////////////////////
// Source:      src/generic/odcombo.cpp
/////////////////////////////////////////////////////////////////////////////

#include "FbComboBox.h"
#include "FbTreeModel.h"

//-----------------------------------------------------------------------------
//  FbCustomCombo
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCustomCombo, wxComboCtrl)

//-----------------------------------------------------------------------------
//  FbComboPopup
//-----------------------------------------------------------------------------

// time in milliseconds before partial completion buffer drops
#define fbODCB_PARTIAL_COMPLETION_TIME 1000

BEGIN_EVENT_TABLE(FbComboPopup, wxVListBox)
	EVT_MOTION(FbComboPopup::OnMouseMove)
	EVT_KEY_DOWN(FbComboPopup::OnKey)
	EVT_CHAR(FbComboPopup::OnChar)
	EVT_LEFT_UP(FbComboPopup::OnLeftClick)
END_EVENT_TABLE()


void FbComboPopup::Init()
{
	m_itemHeight = 0;
	m_value = -1;
	m_itemHover = -1;
	m_clientDataItemsType = wxClientData_None;
	m_partialCompletionString = wxEmptyString;
	m_model = NULL;
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

	// TODO: Move this to SetFont
	m_itemHeight = GetCharHeight() + 0;

	wxVListBox::SetItemCount(GetCount());

	return true;
}

FbComboPopup::~FbComboPopup()
{
	wxDELETE(m_model);
}

void FbComboPopup::SetFocus()
{
	// Suppress SetFocus() warning by simply not calling it. This combo popup
	// has already been designed with the assumption that SetFocus() may not
	// do anything useful, so it really doesn't need to be called.
#ifdef __WXMSW__
	//
#else
	wxVListBox::SetFocus();
#endif
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

void FbComboPopup::OnDrawBg( wxDC& dc, const wxRect& rect, int item, int flags ) const
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
void FbComboPopup::OnDrawItem( wxDC& dc, const wxRect& rect, int index, int flags ) const
{
	FbComboBox * combo = wxDynamicCast(m_combo, FbComboBox);
	FbModelItem item = m_model->GetData(index + 1);
	if (m_model && combo && item) combo->OnDrawItem(dc, rect, index, item, flags);
}

void FbComboPopup::DismissWithEvent()
{
	StopPartialCompletion();

	int selection = wxVListBox::GetSelection();

	Dismiss();

	wxString valStr = GetString(selection);

	m_value = selection;
	
	if (m_model) m_model->FindRow(selection + 1, true);

	if ( valStr != m_combo->GetValue() )
		m_combo->SetValueWithEvent(valStr);

	SendComboBoxEvent(selection);
}

void FbComboPopup::SendComboBoxEvent( int selection )
{
	wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED, m_combo->GetId());

	evt.SetEventObject(m_combo);

	evt.SetInt(selection);

	m_combo->GetEventHandler()->AddPendingEvent(evt);
}

// returns true if key was consumed
bool FbComboPopup::HandleKey( int keycode, bool saturate, wxChar keychar )
{
	const int itemCount = GetCount();

	// keys do nothing in the empty control and returning immediately avoids
	// using invalid indices below
	if ( !itemCount )
		return false;

	int value = m_value;
	int comboStyle = m_combo->GetWindowStyle();

	if ( keychar > 0 )
	{
		// we have character equivalent of the keycode; filter out these that
		// are not printable characters
		if ( !wxIsprint(keychar) )
			keychar = 0;
	}

	if ( keycode == WXK_DOWN || keycode == WXK_NUMPAD_DOWN || keycode == WXK_RIGHT )
	{
		value++;
		StopPartialCompletion();
	}
	else if ( keycode == WXK_UP || keycode == WXK_NUMPAD_UP || keycode == WXK_LEFT )
	{
		value--;
		StopPartialCompletion();
	}
	else if ( keycode == WXK_PAGEDOWN || keycode == WXK_NUMPAD_PAGEDOWN )
	{
		value+=10;
		StopPartialCompletion();
	}
	else if ( keycode == WXK_PAGEUP || keycode == WXK_NUMPAD_PAGEUP )
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
		m_combo->SetValue(GetString(value));

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
	if ( !HandleKey(event.GetKeyCode(), true) )
		event.Skip();
}

void FbComboPopup::OnComboCharEvent( wxKeyEvent& event )
{
	// unlike in OnComboKeyEvent, wxEVT_CHAR contains meaningful
	// printable character information, so pass it
#if wxUSE_UNICODE
	const wxChar charcode = event.GetUnicodeKey();
#else
	const wxChar charcode = (wxChar)event.GetKeyCode();
#endif

	if ( !HandleKey(event.GetKeyCode(), true, charcode) )
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
		// completion is handled in OnChar() below
		event.Skip();
	}
}

void FbComboPopup::OnChar(wxKeyEvent& event)
{
	if ( m_combo->GetWindowStyle() & wxCB_READONLY )
	{
		// Process partial completion key codes here, but not the arrow keys as
		// the base class will do that for us
#if wxUSE_UNICODE
		const wxChar charcode = event.GetUnicodeKey();
#else
		const wxChar charcode = (wxChar)event.GetKeyCode();
#endif
		if ( wxIsprint(charcode) )
		{
			OnComboCharEvent(event);
			SetSelection(m_value); // ensure the highlight bar moves
			return; // don't skip the event
		}
	}

	event.Skip();
}

wxString FbComboPopup::GetString( int item ) const
{
	return m_model ? m_model->GetData(item + 1) : wxString();
}

wxString FbComboPopup::GetStringValue() const
{
	return wxEmptyString;
}

void FbComboPopup::SetSelection( int item )
{
	wxCHECK_RET( item == wxNOT_FOUND || ((unsigned int)item < GetCount()),
				 wxT("invalid index in FbComboPopup::SetSelection") );

	m_value = item;

	if ( IsCreated() ) {
		wxVListBox::SetSelection(item);
		if (m_model) m_model->FindRow(item + 1, true);
	}
}

wxCoord FbComboPopup::OnMeasureItem(size_t n) const
{
    FbComboBox * combo = wxDynamicCast(m_combo, FbComboBox);
	wxCoord h = combo ? combo->OnMeasureItem(n) : -1;
	return h > 0 ? h : m_itemHeight;
}

int FbComboPopup::GetSelection() const
{
	return m_value;
}

wxSize FbComboPopup::GetAdjustedSize( int minWidth, int prefHeight, int maxHeight )
{
	int height = 250;

	maxHeight -= 2;  // Must take borders into account

	if ( GetCount() )
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

	// Take scrollbar into account in width calculations
	return wxSize(minWidth, height + 2);
}

void FbComboPopup::AssignModel(FbModel * model)
{
	wxDELETE(m_model);
	m_model = model;

	if (m_itemHeight)
		wxVListBox::SetItemCount(GetCount());

	m_value = model ? model->GetPosition() - 1 : -1;
}

unsigned int FbComboPopup::GetCount() const
{
	return m_model ? m_model->GetRowCount() : 0;
}

// ----------------------------------------------------------------------------
// FbComboBox
// ----------------------------------------------------------------------------


IMPLEMENT_CLASS(FbComboBox, wxComboCtrl)

BEGIN_EVENT_TABLE(FbComboBox, wxComboCtrl)
END_EVENT_TABLE()

void FbComboBox::Init()
{
	m_initModel = NULL;
}

FbComboBox::~FbComboBox()
{
	wxDELETE(m_initModel);
}

void FbComboBox::DoSetPopupControl(wxComboPopup* popup)
{
	if ( popup == NULL) popup = new FbComboPopup();

	wxComboCtrl::DoSetPopupControl(popup);

	wxASSERT(popup);

	// Add initial choices to the wxVListBox
	if ( m_initModel )
	{
		GetVListBoxComboPopup()->AssignModel(m_initModel);
		m_initModel = NULL;
	}
}

// ----------------------------------------------------------------------------
// FbComboBox item drawing and measuring default implementations
// ----------------------------------------------------------------------------

void FbComboBox::OnDrawItem( wxDC& dc, const wxRect& rect, int index, FbModelItem item, int flags ) const
{
	if ( flags & wxODCB_PAINTING_CONTROL )
	{
		dc.DrawText( GetValue(),
					 rect.x + GetTextIndent(),
					 (rect.height-dc.GetCharHeight())/2 + rect.y );
	}
	else
	{
		int x = rect.x + 2 + rect.height * item.Level(); 
		dc.DrawText( item[0], x, rect.y );
	}
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

void FbComboBox::AssignModel(FbModel * model)
{
	FbComboPopup * popup = GetVListBoxComboPopup();
	if (popup) {
		popup->AssignModel(model);
	} else {
		wxDELETE(m_initModel);
		m_initModel = model;
	}
}

FbModelItem FbComboBox::GetCurrent()
{
	FbModel * model = GetModel();
	return model ? model->GetCurrent() : FbModelItem();
}

FbModel * FbComboBox::GetModel() const
{
	FbComboPopup * popup = GetVListBoxComboPopup();
	return popup ? popup->GetModel() : m_initModel;
}

wxString FbComboBox::GetText()
{
	return GetCurrent()[0];
}
