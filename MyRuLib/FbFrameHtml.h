#ifndef __FBFRAMEHTML_H__
#define __FBFRAMEHTML_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbBookData.h"
#include "FbFrameBase.h"

class FbFrameHtml: public wxAuiMDIChildFrame
{
public:
	FbFrameHtml(wxAuiMDIParentFrame * parent, BookTreeItemData & data);
	void Load(const wxString & html);
	static void Execute();
protected:
	virtual void CreateControls();
private:
	wxHtmlWindow m_info;
	int m_id;
	wxString m_type;
private:
    void OnInfoUpdate(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEHTML_H__
