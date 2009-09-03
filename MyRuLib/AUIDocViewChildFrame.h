#ifndef _AUI_DOCVIEW_CHILDFRAME_H
#define _AUI_DOCVIEW_CHILDFRAME_H

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>

class AUIDocViewChildFrame : public wxAuiMDIChildFrame
{
	bool m_HasToolBar;
	void CreateControls();
	wxToolBar * m_ToolBar;
	wxMenuBar * CreateMenuBar();
public:
	AUIDocViewChildFrame();
	AUIDocViewChildFrame(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, 
		const wxString & title = wxEmptyString, bool has_toolbar = true);
	bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, 
		const wxString & title = wxEmptyString, bool has_toolbar = true);

	virtual wxToolBar* CreateToolBar(long style,
                                     wxWindowID winid,
                                     const wxString& WXUNUSED(name));        
    virtual wxToolBar *GetToolBar() const;
private:
	void OnToolButton(wxCommandEvent & event);
	void OnToggleToolbar(wxCommandEvent & event);
	void OnActivated(wxActivateEvent & event);	
};

#endif
