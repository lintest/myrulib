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

class FbFrameSearch : public FbFrameBase
{
public:
    FbFrameSearch(): FbFrameBase() {};
	FbFrameSearch(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
    void FillByFind(const wxString &title, const wxString &author = wxEmptyString);
protected:
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	virtual void CreateControls();
private:
    void CreateBookInfo();
private:
    wxTextCtrl * m_textTitle;
    wxTextCtrl * m_textAuthor;
private:
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMESEARCH_H__
