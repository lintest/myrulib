#ifndef __BOOKLISTCTRL_H__
#define __BOOKLISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>

class BookListCtrl: public wxTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style);
	void SelectAll(int iImageIndex = 1);
    virtual void AddColumn(const wxString& text, int width, int flag = wxALIGN_LEFT);
private:
    void SelectChild(const wxTreeItemId &parent, int iImageIndex = 1);
    void ShowContextMenu(const wxPoint& pos);
private:
    void OnContextMenu(wxTreeEvent& event);
	void OnImageClick(wxTreeEvent &event);
    void OnSelectAll(wxCommandEvent& event);
    void OnUnselectAll(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
private:
	wxArrayInt colSizes;
	DECLARE_EVENT_TABLE()
};

#endif // __BOOKLISTCTRL_H__
