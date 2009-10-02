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
    virtual void Update();
private:
	void DoResizeCols(int width);
private:
	wxArrayInt m_ColSizes;
private:
    void OnSizing(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBTREELISTCTRL_H__
