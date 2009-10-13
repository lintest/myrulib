#ifndef __FBFRAMEINFO_H__
#define __FBFRAMEINFO_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbFrameBase.h"

class FbFrameInfo: public wxAuiMDIChildFrame
{
public:
	FbFrameInfo(wxAuiMDIParentFrame * parent);
	void Load(const wxString & html);
	static void Execute();
protected:
	virtual void CreateControls();
private:
	wxHtmlWindow m_info;
private:
    void OnSave(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEINFO_H__
