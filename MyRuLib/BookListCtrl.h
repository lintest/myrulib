#ifndef __BOOKLISTCTRL_H__
#define __BOOKLISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>

class BookListCtrl: public wxTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style);
	void SelectAll();
    void SelectChild(const wxTreeItemId &parent);
    virtual void AddColumn(const wxString& text, int width, int flag = wxALIGN_LEFT);
private:
	void OnImageClick(wxTreeEvent &event);
    void OnSize(wxSizeEvent& event);
private:
	wxArrayInt colSizes;
	DECLARE_EVENT_TABLE()
};

#endif // __BOOKLISTCTRL_H__
