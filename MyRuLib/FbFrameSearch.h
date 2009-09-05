#ifndef __FBFRAMESEARCH_H__
#define __FBFRAMESEARCH_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include "BookListCtrl.h"

class FbFrameSearch : public wxAuiMDIChildFrame
{
private:
	bool m_HasToolBar;
	void CreateControls();
	wxToolBar * m_ToolBar;
	wxMenuBar * CreateMenuBar();
public:
	FbFrameSearch();
	FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar* CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
    virtual wxToolBar *GetToolBar() const  { return NULL; };
private:
	void OnToolButton(wxCommandEvent & event);
	void OnToggleToolbar(wxCommandEvent & event);
	void OnActivated(wxActivateEvent & event);
    void CreateBookInfo();
private:
    wxTextCtrl* m_textTitle;
    wxTextCtrl* m_textAuthor;
    BookListCtrl * m_BooksListView;
    wxSplitterWindow * m_BooksSplitter;
	wxHtmlWindow * m_BooksInfoPanel;
};

#endif // __FBFRAMESEARCH_H__
