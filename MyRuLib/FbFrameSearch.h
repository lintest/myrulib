#ifndef __FBFRAMESEARCH_H__
#define __FBFRAMESEARCH_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include "FbFrameBase.h"
#include "BooksPanel.h"

class FbFrameSearch : public FbFrameBase
{
public:
    FbFrameSearch(): FbFrameBase() {};
	FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
    BooksPanel * m_BooksPanel;
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
protected:
	virtual void CreateControls();
private:
	void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
    void OnSubmenu(wxCommandEvent& event);
    void CreateBookInfo();
	DECLARE_EVENT_TABLE()
private:
    wxTextCtrl * m_textTitle;
    wxTextCtrl * m_textAuthor;
};

#endif // __FBFRAMESEARCH_H__
