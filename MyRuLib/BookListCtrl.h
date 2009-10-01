#ifndef __BOOKLISTCTRL_H__
#define __BOOKLISTCTRL_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbTreeListCtrl.h"

class BookListCtrl: public FbTreeListCtrl
{
public:
    BookListCtrl(wxWindow *parent, wxWindowID id, long style);
	void SelectAll(int iImageIndex = 1);
	void FillBooks(wxSQLite3ResultSet & result, const wxString &caption);
	wxString GetSelected();
private:
    void SelectChild(const wxTreeItemId &parent, int iImageIndex = 1);
    void ShowContextMenu(const wxPoint& pos);
    void ScanChecked(const wxTreeItemId &root, wxString  &selections);
    void ScanSelected(const wxTreeItemId &root, wxString  &selections);
private:
	void OnImageClick(wxTreeEvent &event);
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
