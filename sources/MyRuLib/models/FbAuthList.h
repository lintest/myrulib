#ifndef __FBAUTHLIST_H__
#define __FBAUTHLIST_H__

#include "controls/FbTreeModel.h"
#include "FbCollection.h"
#include "FbFrameThread.h"

class FbAuthListInfo: public wxObject
{
	public:
		FbAuthListInfo(wxChar letter = 0)
			: m_letter(letter), m_string(wxEmptyString)
				{ if (letter == wxT('*')) m_letter = 0; }
		FbAuthListInfo(const wxString &string)
			: m_letter(0), m_string(string)
				{ if (string.Len() == 1) { m_letter = Upper(string)[0]; m_string.Empty(); } }
		FbAuthListInfo(const FbAuthListInfo & info)
			: m_letter(info.m_letter), m_string(info.m_string) {}
		bool IsFullText() const
			{ return FbSearchFunction::IsFullText(m_string); }
		operator bool() const
			{ return m_letter || !m_string.IsEmpty(); }
	private:
		wxChar m_letter;
		wxString m_string;
		friend class FbAuthListThread;
};

class FbAuthListThread : public FbFrameThread
{
	public:
		FbAuthListThread(wxEvtHandler * frame, const FbAuthListInfo &info, int order, const wxString & filename)
			: FbFrameThread(frame, filename), m_info(info), m_order(order) {}
	protected:
		virtual void * Entry();
		void MakeModel(FbSQLite3ResultSet &result);
	private:
		wxString GetJoin();
		wxString GetOrder();
		void DoAuthor(FbSQLite3Database &database);
		void DoLetter(FbSQLite3Database &database);
		void DoString(FbSQLite3Database &database);
		void DoFullText(FbSQLite3Database &database);
		FbAuthListInfo m_info;
		const int m_order;
};

class FbAuthListData: public FbModelData
{
	public:
		FbAuthListData(int code): m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col = 0) const;
		int GetCode() const { return m_code; }
		virtual FbModelData * Clone() const
			 { return new FbAuthListData(m_code); }
		FbMasterInfo GetInfo(FbModel & model) const;
	private:
		int m_code;
		DECLARE_CLASS(FbAuthListData);
};

class FbAuthListModel: public FbListModel
{
	public:
		FbAuthListModel(const wxArrayInt &items, int code = 0);
		virtual ~FbAuthListModel(void);
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
		FbSQLite3Database m_database;
		DECLARE_CLASS(FbAuthListModel);
};

#endif // __FBAUTHLIST_H__
