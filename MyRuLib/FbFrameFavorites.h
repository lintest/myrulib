#ifndef __FBFRAMEFAVORITES_H__
#define __FBFRAMEFAVORITES_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbFrameBase.h"

class FbFrameFavorites : public FbFrameBase
{
public:
    FbFrameFavorites(): FbFrameBase() {};
	FbFrameFavorites(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
//	bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
protected:
	virtual void CreateControls();
private:
	void OnToolButton(wxCommandEvent & event);
	void OnToggleToolbar(wxCommandEvent & event);
	void OnActivated(wxActivateEvent & event);
};

#endif // __FBFRAMEFAVORITES_H__
