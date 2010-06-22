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
		virtual FbModelData * Clone() const
			 { return new FbBookListData(m_code); }
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
		virtual size_t GetSelected(wxArrayInt &items);
	public:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelItem GetCurrent()
			{ return GetData(m_position); };

		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete() {}
		virtual void SelectAll(bool value);

	protected:
		virtual FbModelItem DoGetData(size_t row, int &level);
	private:
        wxString GetSQL(const wxString & order, const wxString & condition);
	private:
		wxArrayInt m_items;
		FbSortedArrayInt m_check;
		DECLARE_CLASS(FbBookListModel);
};

#endif // __FBBOOKLIST_H__
