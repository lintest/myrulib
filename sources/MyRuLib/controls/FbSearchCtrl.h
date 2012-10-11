/////////////////////////////////////////////////////////////////////////////
// Name:        srchctrl.h
// Purpose:     wxSearchCtrlBase class
// Author:      Vince Harron
// Created:     2006-02-18
// RCS-ID:      $Id: srchctrl.h 45828 2007-05-05 14:51:51Z VZ $
// Copyright:   (c) Vince Harron
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __SEARCHCTRL_H__
#define __SEARCHCTRL_H__

#include <wx/wx.h>
#include <wx/odcombo.h>
#include <wx/srchctrl.h>

#ifndef __WXGTK__
	#define FB_SEARCH_COMBO_CTRL
#endif // __WXMSW__

#ifdef FB_SEARCH_COMBO_CTRL

class FbSearchCtrl : public wxOwnerDrawnComboBox
{
public:
	FbSearchCtrl();

	FbSearchCtrl(wxWindow *parent, wxWindowID id,
			   const wxString& value = wxEmptyString,
			   const wxPoint& pos = wxDefaultPosition,
			   const wxSize& size = wxDefaultSize,
			   long style = 0,
			   const wxValidator& validator = wxDefaultValidator,
			   const wxString& name = wxSearchCtrlNameStr);

	virtual ~FbSearchCtrl();

	void Init();

	bool Create(wxWindow *parent, wxWindowID id,
				const wxString& value = wxEmptyString,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0,
				const wxValidator& validator = wxDefaultValidator,
				const wxString& name = wxSearchCtrlNameStr);

	virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) {}

	void DoEvent(wxEvent& event) {
		GetEventHashTable().HandleEvent(event, this);
	}

	virtual void OnButtonClick() {
		GetTextCtrl()->Clear();
	}

	virtual void SelectAll() {
		GetTextCtrl()->SelectAll();
	}

protected:
	virtual wxBitmap RenderCancelBitmap( int x, int y );

private:
	DECLARE_CLASS(FbSearchCtrl)
};

#else // FB_SEARCH_COMBO_CTRL

// ----------------------------------------------------------------------------
// a search ctrl is a text control with a search button and a cancel button
// it is based on the MacOSX 10.3 control HISearchFieldCreate
// ----------------------------------------------------------------------------

class FbSearchTextCtrl;
class FbSearchButton;

class WXDLLEXPORT FbSearchCtrlBase : public wxTextCtrlBase
{
public:
    FbSearchCtrlBase() { }
    virtual ~FbSearchCtrlBase() { }

    // get/set options
    virtual void ShowSearchButton( bool show ) = 0;
    virtual bool IsSearchButtonVisible() const = 0;

    virtual void ShowCancelButton( bool show ) = 0;
    virtual bool IsCancelButtonVisible() const = 0;
};

// ----------------------------------------------------------------------------
// FbSearchCtrl is a combination of wxTextCtrl and wxSearchButton
// ----------------------------------------------------------------------------

class WXDLLEXPORT FbSearchCtrl : public FbSearchCtrlBase
{
public:
    // creation
    // --------

    FbSearchCtrl();
    FbSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxSearchCtrlNameStr);

    virtual ~FbSearchCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSearchCtrlNameStr);

	void DoEvent(wxEvent& event) {
		GetEventHashTable().HandleEvent(event, this);
	}

    // get/set search options
    // ----------------------
    virtual void ShowSearchButton( bool show );
    virtual bool IsSearchButtonVisible() const;

    virtual void ShowCancelButton( bool show );
    virtual bool IsCancelButtonVisible() const;

#if wxABI_VERSION >= 20802
    // TODO: In 2.9 these should probably be virtual, and declared in the base class...
    void SetDescriptiveText(const wxString& text);
    wxString GetDescriptiveText() const;
#endif

    // accessors
    // ---------

    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);

    virtual wxString GetRange(long from, long to) const;

    virtual int GetLineLength(long lineNo) const;
    virtual wxString GetLineText(long lineNo) const;
    virtual int GetNumberOfLines() const;

    virtual bool IsModified() const;
    virtual bool IsEditable() const;

    // more readable flag testing methods
    virtual bool IsSingleLine() const;
    virtual bool IsMultiLine() const;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const;

    virtual wxString GetStringSelection() const;

    // operations
    // ----------

    // editing
    virtual void Clear();
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);

    // load/save the controls contents from/to the file
    virtual bool LoadFile(const wxString& file);
    virtual bool SaveFile(const wxString& file = wxEmptyString);

    // sets/clears the dirty flag
    virtual void MarkDirty();
    virtual void DiscardEdits();

    // set the max number of characters which may be entered in a single line
    // text control
    virtual void SetMaxLength(unsigned long WXUNUSED(len));

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    virtual void WriteText(const wxString& text);
    virtual void AppendText(const wxString& text);

    // insert the character which would have resulted from this key event,
    // return true if anything has been inserted
    virtual bool EmulateKeyPress(const wxKeyEvent& event);

    // text control under some platforms supports the text styles: these
    // methods allow to apply the given text style to the given selection or to
    // set/get the style which will be used for all appended text
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);
    virtual bool GetStyle(long position, wxTextAttr& style);
    virtual bool SetDefaultStyle(const wxTextAttr& style);
    virtual const wxTextAttr& GetDefaultStyle() const;

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    virtual long XYToPosition(long x, long y) const;
    virtual bool PositionToXY(long pos, long *x, long *y) const;

    virtual void ShowPosition(long pos);

    // find the character at position given in pixels
    //
    // NB: pt is in device coords (not adjusted for the client area origin nor
    //     scrolling)
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt, long *pos) const;
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt, wxTextCoord *col, wxTextCoord *row) const;

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;

    // Undo/redo
    virtual void Undo();
    virtual void Redo();

    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // Insertion point
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;

    virtual void SetSelection(long from, long to);
    virtual void SelectAll();
    virtual void SetEditable(bool editable);

    // do the window-specific processing after processing the update event
    virtual void DoUpdateWindowUI(wxUpdateUIEvent& event);

    virtual bool ShouldInheritColours() const;

    // wxWindow overrides
    virtual bool SetFont(const wxFont& font);

    // search control generic only
    void SetSearchBitmap( const wxBitmap& bitmap );
    void SetCancelBitmap( const wxBitmap& bitmap );

protected:
    virtual void DoSetValue(const wxString& value, int flags = 0);

    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual void LayoutControls(int x, int y, int width, int height);

    virtual void RecalcBitmaps();

    void Init();

    virtual wxBitmap RenderSearchBitmap( int x, int y, bool renderDrop );
    virtual wxBitmap RenderCancelBitmap( int x, int y );

    virtual void OnSearchButton( wxCommandEvent& event );
    virtual void OnCancelButton( wxCommandEvent& event );

    void OnSetFocus( wxFocusEvent& event );
    void OnSize( wxSizeEvent& event );

private:
    friend class FbSearchButton;

    // the subcontrols
    FbSearchTextCtrl *m_text;
    FbSearchButton *m_searchButton;
    FbSearchButton *m_cancelButton;

    bool m_searchButtonVisible;
    bool m_cancelButtonVisible;

    bool m_searchBitmapUser;
    bool m_cancelBitmapUser;

    wxBitmap m_searchBitmap;
    wxBitmap m_cancelBitmap;

private:
    DECLARE_DYNAMIC_CLASS(FbSearchCtrl)

    DECLARE_EVENT_TABLE()
};

#endif // FB_SEARCH_COMBO_CTRL

#endif // __SEARCHCTRL_H__
