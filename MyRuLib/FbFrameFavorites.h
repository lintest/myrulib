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
	virtual void UpdateBooklist() {};
private:
	wxHtmlWindow m_Info;
};

#endif // __FBFRAMEFAVORITES_H__
