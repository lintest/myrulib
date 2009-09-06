#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>

class FbFrameBase : public wxAuiMDIChildFrame
{
public:
	FbFrameBase();
	FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
    virtual wxToolBar *GetToolBar() const  { return NULL; };
protected:
	virtual void CreateControls() {};
	virtual wxMenuBar * CreateMenuBar();
private:
	void OnActivated(wxActivateEvent & event);
};

#endif //__FBFRAMEBASE_H__

