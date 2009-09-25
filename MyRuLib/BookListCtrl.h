#ifndef __BOOKLISTCTRL_H__
#define __BOOKLISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>

class BookListCtrl: public wxTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style);
	void SelectAll(int iImageIndex = 1);
    virtual void AddColumn(const wxString& text, int width, int flag = wxALIGN_LEFT);
	void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
private:
    void SelectChild(const wxTreeItemId &parent, int iImageIndex = 1);
    void ShowContextMenu(const wxPoint& pos);
private:
	void OnImageClick(wxTreeEvent &event);
    void OnSizing(wxSizeEvent& event);
private:
	wxArrayInt colSizes;
	DECLARE_EVENT_TABLE()
};

class BookListUpdater
{
	public:
		BookListUpdater(wxTreeListCtrl * list);
		virtual ~BookListUpdater();
	private:
		wxTreeListCtrl * m_list;
};

#endif // __BOOKLISTCTRL_H__
