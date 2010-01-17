#ifndef __FBBOOKLIST_H__
#define __FBBOOKLIST_H__

#include <wx/wx.h>
#include "wx/treelistctrl.h"
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbTreeListCtrl.h"

class FbCheckList: public FbTreeListCtrl
{
	public:
		FbCheckList(wxWindow *parent, wxWindowID id, long style);
		void SelectAll(int iImageIndex = 1);
	private:
		void SelectChild(const wxTreeItemId &parent, int iImageIndex = 1);
	private:
		void OnKeyDown(wxTreeEvent & event);
		void OnCollapsing(wxTreeEvent & event);
		void OnImageClick(wxTreeEvent &event);
		DECLARE_EVENT_TABLE();
};

class FbBookList: public FbCheckList
{
	public:
		FbBookList(wxWindow *parent, wxWindowID id, long style);
		wxString GetSelected();
		size_t GetCount();
		size_t GetSelected(wxString &selections);
		size_t GetSelected(wxArrayInt &items);
		void DeleteItems(wxArrayInt &items);
	private:
		size_t GetCount(const wxTreeItemId &parent, wxArrayInt &items);
		bool DeleteItems(const wxTreeItemId &parent, wxArrayInt &items);
		void ShowContextMenu(const wxPoint& pos);
		size_t ScanChecked(const wxTreeItemId &parent, wxString  &selections);
		size_t ScanSelected(const wxTreeItemId &parent, wxString  &selections);
		void ScanChecked(const wxTreeItemId &parent, wxArrayInt &items);
		void ScanSelected(const wxTreeItemId &parent, wxArrayInt &items);
};

class BookListUpdater
{
	public:
		BookListUpdater(wxTreeListCtrl * list);
		virtual ~BookListUpdater();
	private:
		wxTreeListCtrl * m_list;
};

#endif // __FBBOOKLIST_H__
