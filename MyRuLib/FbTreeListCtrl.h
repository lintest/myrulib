#ifndef __FBTREELISTCTRL_H__
#define __FBTREELISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>

class FbTreeListCtrl: public wxTreeListCtrl
{
public:
    FbTreeListCtrl(wxWindow *parent, wxWindowID id, long style);
    virtual void AddColumn(const wxString& text, int width, int flag = wxALIGN_LEFT);
    void EmptyCols();
private:
    void OnSizing(wxSizeEvent& event);
private:
	wxArrayInt m_ColSizes;
	DECLARE_EVENT_TABLE()
};

#endif // __FBTREELISTCTRL_H__
