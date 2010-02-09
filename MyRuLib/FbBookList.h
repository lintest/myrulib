#ifndef __FBBOOKLIST_H__
#define __FBBOOKLIST_H__

#include "FbCheckList.h"

enum FbListMode {
	FB2_MODE_LIST = 0,
	FB2_MODE_TREE = 1,
};

enum FbViewMode {
	FB2_VIEW_HORISONTAL = 0,
	FB2_VIEW_VERTICAL = 1,
	FB2_VIEW_NOTHING = 2,
};

class FbBookList: public FbCheckList
{
	public:
		FbBookList(wxWindow *parent, wxWindowID id, long style);
		wxString GetSelected();
		int GetItemBook(const wxTreeItemId &item) const;
		size_t GetCount();
		size_t GetSelected(wxString &selections);
		size_t GetSelected(wxArrayInt &items);
		void DeleteItems(wxArrayInt &items);
		size_t UpdateRating(size_t column, const wxString &rating);
	private:
		size_t GetCount(const wxTreeItemId &parent, wxArrayInt &items);
		bool DeleteItems(const wxTreeItemId &parent, wxArrayInt &items);
		void ShowContextMenu(const wxPoint& pos);
		size_t GetChecked(const wxTreeItemId &parent, wxString  &selections);
		size_t GetChecked(const wxTreeItemId &parent, wxArrayInt &items);
		size_t UpdateRating(wxTreeItemId parent, size_t column, const wxString &rating);
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
