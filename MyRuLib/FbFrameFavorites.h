#ifndef __FBFRAMEFAVORITES_H__
#define __FBFRAMEFAVORITES_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>

class FbFrameFavorites : public wxAuiMDIChildFrame
{
	bool m_HasToolBar;
	void CreateControls();
	wxToolBar * m_ToolBar;
	wxMenuBar * CreateMenuBar();
public:
	FbFrameFavorites();
	FbFrameFavorites(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar* CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
    virtual wxToolBar *GetToolBar() const  { return NULL; };
private:
	void OnToolButton(wxCommandEvent & event);
	void OnToggleToolbar(wxCommandEvent & event);
	void OnActivated(wxActivateEvent & event);
};

#endif // __FBFRAMEFAVORITES_H__
