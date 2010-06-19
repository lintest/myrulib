#ifndef __FBBOOKLIST_H__
#define __FBBOOKLIST_H__

#include "FbTreeModel.h"
#include "FbCollection.h"
#include "FbThread.h"
#include "FbBookEvent.h"

class FbBookListData: public FbModelData
{
	public:
		FbBookListData(int code): m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
	protected:
		virtual void DoSetState(FbModel & model, int state) {};
		virtual int DoGetState(FbModel & model) const { return -1; };
	private:
		int m_code;
		DECLARE_CLASS(FbBookListData);
};

class FbBookListModel: public FbListModel
{
	public:
		FbBookListModel(const wxArrayInt &items);
		FbBookListModel(int order, wxChar letter = 0);
		FbBookListModel(int order, const wxString &mask);
		virtual ~FbBookListModel(void);
		virtual void Append(FbModelData * data);
		virtual void Replace(FbModelData * data);
		virtual void Delete();
		void Append(const wxArrayInt &items);
		void Delete(int code);
	public:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); };
	protected:
		virtual FbModelData * DoGetData(size_t row, int &level);
	private:
        wxString GetSQL(const wxString & order, const wxString & condition);
	private:
		wxArrayInt m_items;
		FbSortedArrayInt m_check;
		FbBookListData * m_data;
		DECLARE_CLASS(FbBookListModel);
};

//=================================================================//

#include "FbCheckList.h"

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
