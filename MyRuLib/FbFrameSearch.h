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
#include "FbFrameBase.h"

class FbFrameSearch : public FbFrameBase
{
public:
    FbFrameSearch(): FbFrameBase() {};
	FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
protected:
	virtual void CreateControls();
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
