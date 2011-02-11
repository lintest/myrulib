/////////////////////////////////////////////////////////////////////////////
// Source:      wx/odcombo.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __FBCOMBOBOX_H__
#define __FBCOMBOBOX_H__

#include <wx/defs.h>
#include <wx/combo.h>
#include <wx/ctrlsub.h>
#include <wx/vlbox.h>
#include <wx/timer.h>
#include <wx/odcombo.h>

class FbFileSelectorCombo : public wxComboCtrl
{
public:
    FbFileSelectorCombo() : wxComboCtrl() { Init(); }

    FbFileSelectorCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        //
        // Prepare custom button bitmap (just '...' text)
        wxMemoryDC dc;
        wxBitmap tmp;
        dc.SelectObject(tmp);

        wxString str = wxChar(0x2026);
        wxSize s = dc.GetTextExtent(str);
        wxBitmap bmp(s.x + 2, s.y + 2);
        dc.SelectObject(bmp);

        // Draw transparent background
        wxColour colour(255, 0, 255);
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

    virtual void OnButtonClick()
    {
    	wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED,  GetId());
    	wxPostEvent(this, event);
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup))
    {
    }

private:
    // Initialize member variables here
    void Init()
    {
    }
};

class FbSearchCombo: public wxComboCtrl
{
	public:
		FbSearchCombo() : wxComboCtrl() { Init(); }

		FbSearchCombo(wxWindow *parent,
							wxWindowID id = wxID_ANY,
							const wxString& value = wxEmptyString,
							const wxPoint& pos = wxDefaultPosition,
							const wxSize& size = wxDefaultSize,
							long style = 0,
							const wxValidator& validator = wxDefaultValidator,
							const wxString& name = wxComboBoxNameStr)
			: wxComboCtrl()
		{
			Init();
			Create(parent,id,value,
				   pos,size,
				   // Style flag wxCC_STD_BUTTON makes the button
				   // behave more like a standard push button.
				   style | wxCC_STD_BUTTON,
				   validator,name);

			SetButtonBitmaps(RenderButtonBitmap(), true);
		}

		virtual void OnButtonClick()
		{
    		wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED,  GetId());
    		wxPostEvent(this, event);
		}

		virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {}

	private:
		// Initialize member variables here
		void Init() {}
	private:
		wxBitmap RenderButtonBitmap();
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
    virtual wxWindow *GetControl() { return this; }
    virtual void SetStringValue( const wxString& value );
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
    void Insert( const wxString& item, int pos );
    int Append(const wxString& item);
    void Clear();
    void Delete( unsigned int item );
    void SetItemClientData(unsigned int n, void* clientData, wxClientDataType clientDataItemsType);
    void *GetItemClientData(unsigned int n) const;
    void SetString( int item, const wxString& str );
    wxString GetString( int item ) const;
    unsigned int GetCount() const;
    int FindString(const wxString& s, bool bCase = false) const;
    int GetSelection() const;

    //void Populate( int n, const wxString choices[] );
    void Populate( const wxArrayString& choices );
    void ClearClientDatas();

    // helpers
    int GetItemAtPosition( const wxPoint& pos ) { return HitTest(pos); }
    wxCoord GetTotalHeight() const { return EstimateTotalHeight(); }
    wxCoord GetLineHeight(int line) const { return OnGetLineHeight(line); }

protected:

    // Called by OnComboDoubleClick and OnComboKeyEvent
    bool HandleKey( int keycode, bool saturate, wxChar unicode = 0 );

    // sends combobox select event from the parent combo control
    void SendComboBoxEvent( int selection );

    // gets value, sends event and dismisses
    void DismissWithEvent();

    // OnMeasureItemWidth will be called on next GetAdjustedSize.
    void ItemWidthChanged(unsigned int item)
    {
        m_widths[item] = -1;
        m_widthsDirty = true;
    }

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
    virtual wxCoord OnMeasureItemWidth( size_t item ) const;

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
    void OnLeftClick(wxMouseEvent& event);

    // Return the widest item width (recalculating it if necessary)
    int GetWidestItemWidth() { CalcWidths(); return m_widestWidth; }

    // Return the index of the widest item (recalculating it if necessary)
    int GetWidestItem() { CalcWidths(); return m_widestItem; }

    // Stop partial completion (when some other event occurs)
    void StopPartialCompletion();

    wxArrayString           m_strings;
    wxArrayPtrVoid          m_clientDatas;

    wxFont                  m_useFont;

    //wxString                m_stringValue; // displayed text (may be different than m_strings[m_value])
    int                     m_value; // selection
    int                     m_itemHover; // on which item the cursor is
    int                     m_itemHeight; // default item height (calculate from font size
                                          // and used in the absence of callback)
    wxClientDataType        m_clientDataItemsType;

private:

    // Cached item widths (in pixels).
    wxArrayInt              m_widths;

    // Width of currently widest item.
    int                     m_widestWidth;

    // Index of currently widest item.
    int                     m_widestItem;

    // Measure some items in next GetAdjustedSize?
    bool                    m_widthsDirty;

    // Find widest item in next GetAdjustedSize?
    bool                    m_findWidest;

    // has the mouse been released on this control?
    bool                    m_clicked;

    // Recalculate widths if they are dirty
    void CalcWidths();

    // Partial completion string
    wxString                m_partialCompletionString;

#if wxUSE_TIMER
    // Partial completion timer
    wxTimer                 m_partialCompletionTimer;
#endif // wxUSE_TIMER

    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// FbComboBox: a generic wxComboBox that allows custom paint items
// in addition to many other types of customization already allowed by
// the wxComboCtrl.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV FbComboBox : public wxComboCtrl,
                                             public wxItemContainer
{
    //friend class wxComboPopupWindow;
    friend class FbComboPopup;
public:

    // ctors and such
    FbComboBox() : wxComboCtrl() { Init(); }

    FbComboBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos,
                         const wxSize& size,
                         int n,
                         const wxString choices[],
                         long style = 0,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();

        (void)Create(parent, id, value, pos, size, n,
                     choices, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    FbComboBox(wxWindow *parent,
                         wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxArrayString& choices,
                         long style,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                int n,
                const wxString choices[],
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~FbComboBox();

    // Prevent app from using wxComboPopup
    void SetPopupControl(FbComboPopup* popup)
    {
        DoSetPopupControl(popup);
    }

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(unsigned int n);
    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    virtual void SetSelection(int n) { Select(n); }


    // Prevent a method from being hidden
    virtual void SetSelection(long from, long to)
    {
        wxComboCtrl::SetSelection(from,to);
    }

    // Return the widest item width (recalculating it if necessary)
    virtual int GetWidestItemWidth() { EnsurePopupControl(); return GetVListBoxComboPopup()->GetWidestItemWidth(); }

    // Return the index of the widest item (recalculating it if necessary)
    virtual int GetWidestItem() { EnsurePopupControl(); return GetVListBoxComboPopup()->GetWidestItem(); }

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

protected:

    // Callback for drawing. Font, background and text colour have been
    // prepared according to selection, focus and such.
    // item: item index to be drawn, may be wxNOT_FOUND when painting combo control itself
    //       and there is no valid selection
    // flags: wxODCB_PAINTING_CONTROL is set if painting to combo control instead of list
    virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const;

    // Callback for item height, or -1 for default
    virtual wxCoord OnMeasureItem( size_t item ) const;

    // Callback for item width, or -1 for default/undetermined
    virtual wxCoord OnMeasureItemWidth( size_t item ) const;

    // Callback for background drawing. Flags are same as with
    // OnDrawItem.
    virtual void OnDrawBackground( wxDC& dc, const wxRect& rect, int item, int flags ) const;

    // NULL popup can be used to indicate default interface
    virtual void DoSetPopupControl(wxComboPopup* popup);

    // clears all allocated client datas
    void ClearClientDatas();

    FbComboPopup* GetVListBoxComboPopup() const
    {
        return (FbComboPopup*) m_popupInterface;
    }

    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, unsigned int pos);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;

    // temporary storage for the initial choices
    //const wxString*         m_baseChoices;
    //int                     m_baseChoicesCount;
    wxArrayString           m_initChs;

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(FbComboBox)
};

#endif // __FBCOMBOBOX_H__

