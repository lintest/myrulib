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
	static wxString GetMd5sum(const int id);
	wxString GetComments();
private:
	wxHtmlWindow m_info;
	wxTextCtrl* m_Caption;
	wxTextCtrl* m_Comment;
	int m_id;
	wxString m_md5sum;
	wxString m_type;
private:
    void OnComment(wxCommandEvent& event);
    void OnInfoUpdate(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEHTML_H__
