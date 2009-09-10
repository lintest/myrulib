#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "BooksPanel.h"

class FbFrameBase : public wxAuiMDIChildFrame
{
public:
	FbFrameBase();
	FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& WXUNUSED(name)) { return NULL; };
protected:
	virtual void CreateControls() {};
	virtual wxMenuBar * CreateMenuBar();
    BooksPanel m_BooksPanel;
private:
	void OnActivated(wxActivateEvent & event);
	void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
    void OnExternal(wxCommandEvent& event);
    void OnSubmenu(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif //__FBFRAMEBASE_H__

