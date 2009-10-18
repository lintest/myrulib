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
	wxString GetSelected();
	void GetSelected(wxArrayInt &items);
	void DeleteItems(wxArrayInt &items);
private:
	bool DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
    void SelectChild(const wxTreeItemId &parent, int iImageIndex = 1);
    void ShowContextMenu(const wxPoint& pos);
    void ScanChecked(const wxTreeItemId &root, wxString  &selections);
    void ScanSelected(const wxTreeItemId &root, wxString  &selections);
    void ScanChecked(const wxTreeItemId &root, wxArrayInt &items);
    void ScanSelected(const wxTreeItemId &root, wxArrayInt &items);
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
