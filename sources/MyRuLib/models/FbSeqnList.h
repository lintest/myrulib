#ifndef __SEQNLIST_H__
#define __SEQNLIST_H__

#include "controls/FbTreeModel.h"
#include "FbCollection.h"
#include "FbFrameThread.h"

class FbSeqnListThread: public FbFrameThread
{
	public:
		FbSeqnListThread(wxEvtHandler * frame, const wxString &string, int order, const wxString & filename)
			: FbFrameThread(frame, filename), m_string(string), m_order(order) {}
	protected:
		virtual void * Entry();
		void MakeModel(wxSQLite3ResultSet &result);
	private:
		wxString GetJoin();
		wxString GetOrder();
		void DoString(wxSQLite3Database &database);
		void DoFullText(wxSQLite3Database &database);
		bool IsFullText(wxSQLite3Database &database) const;
	private:
		wxString m_string;
		const int m_order;
};

class FbSeqnListData: public FbModelData
{
	public:
		FbSeqnListData(int code): m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
		virtual FbModelData * Clone() const
			 { return new FbSeqnListData(m_code); }
		virtual FbMasterInfo GetInfo(FbModel & model) const;
	private:
		int m_code;
		DECLARE_CLASS(FbSeqnListData);
};

class FbSeqnListModel: public FbListModel
{
	public:
		FbSeqnListModel(const wxArrayInt &items, int code = 0);
		virtual ~FbSeqnListModel(void);
		virtual void Append(FbModelData * data);
		virtual void Replace(FbModelData * data);
		virtual void Delete();
		void Append(const wxArrayInt &items);
		void Delete(int code);
		void SetCount(int code, int count);
		int GetCount(int code);
	public:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelItem GetCurrent()
			{ return GetData(m_position); };
		void SetCounter(const wxString & filename);
	protected:
		virtual FbModelItem DoGetData(size_t row, int &level);
	private:
		wxArrayInt m_items;
		FbIntegerHash m_counter;
		wxSQLite3Database m_database;
		DECLARE_CLASS(FbSeqnListModel);
};

#endif // __SEQNLIST_H__
