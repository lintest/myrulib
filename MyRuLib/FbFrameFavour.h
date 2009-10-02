#ifndef __FBFRAMEFAVOUR_H__
#define __FBFRAMEFAVOUR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "wx/treelistctrl.h"
#include "FbFrameBase.h"
#include "FbTreeListCtrl.h"
#include "FbBookEvent.h"

class FbFrameFavour : public FbFrameBase
{
public:
	FbFrameFavour(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
protected:
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	virtual void CreateControls();
private:
    void CreateBookInfo();
	void FillFolders();
	void FillByFolder(const int iFolder);
private:
    wxListBox * m_FolderList;
private:
    void OnChangeMode(wxCommandEvent& event);
    void OnFolderSelected(wxCommandEvent & event);
    void OnGenreSelected(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEFAVOUR_H__
