#ifndef __FBBOOKLIST_H__
#define __FBBOOKLIST_H__

#include "FbTreeModel.h"
#include "FbBookTypes.h"
#include "FbCollection.h"
#include "FbThread.h"
#include "FbMasterInfo.h"

class FbBookListData: public FbModelData
{
	public:
		FbBookListData(int code)
			: m_code(code) {}
		int GetCode() const
			{ return m_code; }
		virtual wxString GetValue(FbModel & model, size_t col = 0) const
			{ return FbCollection::GetBook(m_code, col); }
	protected:
		virtual void DoSetState(FbModel & model, int state);
		virtual int DoGetState(FbModel & model) const;
	private:
		int m_code;
		DECLARE_CLASS(FbBookListData);
};

class FbBookListModel: public FbListModel
{
	public:
		static int CompareInt(int x, int y) { return x - y; }
		FbBookListModel(const wxArrayInt &items);
		virtual ~FbBookListModel(void);
		void Append(const wxArrayInt &items);
		void SetState(int code, int state);
		int GetState(int code) const;
	public:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); };

		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete() {}

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
