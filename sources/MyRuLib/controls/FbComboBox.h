/////////////////////////////////////////////////////////////////////////////
// Source:      wx/odcombo.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __FBCOMBOBOX_H__
#define __FBCOMBOBOX_H__

#include <wx/wx.h>
#include <wx/odcombo.h>

#include "controls/FbTreeModel.h"

class FbCustomCombo : public wxComboCtrl
{
public:
	FbCustomCombo() : wxComboCtrl() { }

	FbCustomCombo(wxWindow *parent,
						wxWindowID id = wxID_ANY,
						const wxString& value = wxEmptyString,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						long style = 0,
						const wxValidator& validator = wxDefaultValidator,
						const wxString& name = wxComboBoxNameStr)
		: wxComboCtrl()
	{
		Create(parent, id, value, pos, size, style | wxCC_STD_BUTTON, validator, name);
	}

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr)
	{
		bool ok = wxComboCtrl::Create(parent, id, value, pos, size, style | wxCC_STD_BUTTON, validator, name);

		if (ok) {
			// Prepare custom button bitmap (just '...' text)
			wxMemoryDC dc;
			wxBitmap tmp;
			dc.SelectObject(tmp);

			wxString str = wxChar(0x2026);
			wxSize s = dc.GetTextExtent(str);
			wxBitmap bmp(s.x + 2, s.y + 2);
			dc.SelectObject(bmp);

			// Draw transparent background
			wxColour colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
			wxBrush brush(colour);
			dc.SetBrush(brush);
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
			dc.DrawText(str, 0, 0);

			dc.SelectObject(wxNullBitmap);

			// Finalize transparency with a mask
			wxMask *mask = new wxMask(bmp, colour);
			bmp.SetMask(mask);

			SetButtonBitmaps(bmp, true);
		}
		return ok;
	}

	virtual void OnButtonClick()
	{
		wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED,  GetId());
		wxPostEvent(this, event);
	}

	// Implement empty DoSetPopupControl to prevent assertion failure.
	virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup))
	{
	}

	DECLARE_CLASS(FbComboBox)
};

// ----------------------------------------------------------------------------
// FbComboPopup is a wxVListBox customized to act as a popup control.
//
// Notes:
//   FbComboBox uses this as its popup. However, it always derives
//   from native wxComboCtrl. If you need to use this popup with
//   wxGenericComboControl, then remember that vast majority of item manipulation
//   functionality is implemented in the FbComboPopup class itself.
//
// ----------------------------------------------------------------------------


class WXDLLIMPEXP_ADV FbComboPopup : public wxVListBox,
											 public wxComboPopup
{
	friend class FbComboBox;
public:

	// init and dtor
	FbComboPopup() : wxVListBox(), wxComboPopup() { }
	virtual ~FbComboPopup();

	// required virtuals
	virtual void Init();
	virtual bool Create(wxWindow* parent);
	virtual void SetFocus();
	virtual wxWindow *GetControl() { return this; }
	virtual wxString GetStringValue() const;

	// more customization
	virtual void OnPopup();
	virtual wxSize GetAdjustedSize( int minWidth, int prefHeight, int maxHeight );
	virtual void PaintComboControl( wxDC& dc, const wxRect& rect );
	virtual void OnComboKeyEvent( wxKeyEvent& event );
	virtual void OnComboDoubleClick();
	virtual bool LazyCreate();

	// Item management
	void SetSelection( int item );
	wxString GetString( int item ) const;
	unsigned int GetCount() const;
	int GetSelection() const;

	// helpers
	int GetItemAtPosition( const wxPoint& pos ) { return HitTest(pos); }
	wxCoord GetTotalHeight() const { return EstimateTotalHeight(); }
	wxCoord GetLineHeight(int line) const { return OnGetLineHeight(line); }
	virtual wxCoord OnGetLineHeight(int line) const { return -1; }

protected:

	// Called by OnComboDoubleClick and OnComboKeyEvent
	bool HandleKey( int keycode, bool saturate, wxChar unicode = 0 );

	// sends combobox select event from the parent combo control
	void SendComboBoxEvent( int selection );

	// gets value, sends event and dismisses
	void DismissWithEvent();

	// Callbacks for drawing and measuring items. Override in a derived class for
	// owner-drawnness. Font, background and text colour have been prepared according
	// to selection, focus and such.
	//
	// item: item index to be drawn, may be wxNOT_FOUND when painting combo control itself
	//       and there is no valid selection
	// flags: wxODCB_PAINTING_CONTROL is set if painting to combo control instead of list
	// NOTE: If FbComboPopup is used with wxComboCtrl class not derived from
	//       FbComboBox, this method must be overridden.
	virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const;

	// This is same as in wxVListBox
	virtual wxCoord OnMeasureItem( size_t item ) const;

	// Return item width, or -1 for calculating from text extent (default)
	virtual wxCoord OnMeasureItemWidth( size_t item ) const
	{
		return -1;
	}

	// Draw item and combo control background. Flags are same as with OnDrawItem.
	// NB: Can't use name OnDrawBackground because of virtual function hiding warnings.
	virtual void OnDrawBg(wxDC& dc, const wxRect& rect, int item, int flags) const;

	// Additional wxVListBox implementation (no need to override in derived classes)
	virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
	void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;

	// filter mouse move events happening outside the list box
	// move selection with cursor
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnKey(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnLeftClick(wxMouseEvent& event);
	void OnComboCharEvent(wxKeyEvent& event);

	// Stop partial completion (when some other event occurs)
	void StopPartialCompletion();

	wxFont m_useFont;

	int m_value; // selection
	int m_itemHover; // on which item the cursor is
	int m_itemHeight; // default item height (calculate from font size and used in the absence of callback)
	wxClientDataType        m_clientDataItemsType;

private:

	// has the mouse been released on this control?
	bool                    m_clicked;

	// Partial completion string
	wxString                m_partialCompletionString;

#if wxUSE_TIMER
	// Partial completion timer
	wxTimer                 m_partialCompletionTimer;
#endif // wxUSE_TIMER

	DECLARE_EVENT_TABLE()

protected:
	void AssignModel(FbModel * model);
	FbModel * GetModel() { return m_model; };
	FbModel * m_model;
};


// ----------------------------------------------------------------------------
// FbComboBox: a generic wxComboBox that allows custom paint items
// in addition to many other types of customization already allowed by
// the wxComboCtrl.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV FbComboBox : public wxComboCtrl
{
	friend class FbComboPopup;
public:

	// ctors and such
	FbComboBox() : wxComboCtrl()
	{
		Init();
	}

	FbComboBox(wxWindow *parent,
						 wxWindowID id,
						 const wxString& value,
						 const wxPoint& pos,
						 const wxSize& size,
						 long style = 0,
						 const wxValidator& validator = wxDefaultValidator,
						 const wxString& name = wxComboBoxNameStr)
		: wxComboCtrl()
	{
		Init();

		Create(parent, id, value, pos, size, style, validator, name);
	}

	bool Create(wxWindow *parent,
				wxWindowID id,
				const wxString& value = wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator,
				const wxString& name = wxComboBoxNameStr)
	{
		return wxComboCtrl::Create(parent, id, value, pos, size, style, validator, name);
	}

	virtual ~FbComboBox();

	// Prevent app from using wxComboPopup
	void SetPopupControl(FbComboPopup* popup)
	{
		DoSetPopupControl(popup);
	}

	// Prevent a method from being hidden
	virtual void SetSelection(long from, long to)
	{
		wxComboCtrl::SetSelection(from, to);
	}

	virtual wxCoord OnMeasureItem( size_t item ) const
	{
		return -1;
	}

	FbModelItem GetCurrent();

	wxString GetText();

protected:

	// Callback for drawing. Font, background and text colour have been
	// prepared according to selection, focus and such.
	// item: item index to be drawn, may be wxNOT_FOUND when painting combo control itself
	//       and there is no valid selection
	// flags: wxODCB_PAINTING_CONTROL is set if painting to combo control instead of list
	virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int index, FbModelItem item, int flags ) const;

	// Callback for background drawing. Flags are same as with
	// OnDrawItem.
	virtual void OnDrawBackground( wxDC& dc, const wxRect& rect, int item, int flags ) const;

	// NULL popup can be used to indicate default interface
	virtual void DoSetPopupControl(wxComboPopup* popup);

	FbComboPopup* GetVListBoxComboPopup() const
	{
		return (FbComboPopup*) m_popupInterface;
	}

private:
	void Init();

	DECLARE_EVENT_TABLE()

	DECLARE_CLASS(FbComboBox)

public:
	void AssignModel(FbModel * m_model);
	FbModel * GetModel() const;

private:
	// temporary storage for the initial model
	FbModel * m_initModel;
};

#endif // __FBCOMBOBOX_H__

