#include <wx/wx.h>
#include "wx/treelistctrl.h"

class BookListCtrl: public wxTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style)
        :wxTreeListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style) {};
    void OnSize(wxSizeEvent& event);
	void OnImageClick(wxTreeEvent &event);
	DECLARE_EVENT_TABLE()
};

