#ifndef __BOOKLISTCTRL_H__
#define __BOOKLISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>

class BookListCtrl: public wxTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style);
    void OnSize(wxSizeEvent& event);
	void OnImageClick(wxTreeEvent &event);
	void SelectAll();
	wxArrayInt colSizes;
	DECLARE_EVENT_TABLE()
private:
    void SelectChild(const wxTreeItemId &parent);
};

#endif // __BOOKLISTCTRL_H__
