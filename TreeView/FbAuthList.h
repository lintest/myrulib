#ifndef __FBAUTHLIST_H__
#define __FBAUTHLIST_H__

#include "FbTreeModel.h"
#include "FbDatabase.h"
#include "FbCollection.h"

#define ID_MASTER_MODEL 2000

class FbAuthListThread: public wxThread
{
	public:
		FbAuthListThread(wxEvtHandler * frame, wxChar letter = 0, int order = 0)
			:wxThread(wxTHREAD_JOINABLE), m_frame(frame), m_letter(letter), m_order(order) {}
	protected:
		virtual void * Entry();
	private:
		static wxString GetOrder(int column);
		void Load(wxSQLite3Database &database);
		wxEvtHandler * m_frame;
		wxChar m_letter;
		int m_order;
};


class FbAuthListData: public FbModelData
{
	public:
		FbAuthListData(int code): m_code(code) {}
		virtual wxString GetValue(FbModel & model, size_t col) const;
		int GetCode() const { return m_code; }
	private:
		int m_code;
		DECLARE_CLASS(FbAuthListData);
};

class FbAuthListModel: public FbListModel
{
	public:
		FbAuthListModel(wxArrayInt &items);
		FbAuthListModel(int order, wxChar letter = 0);
		FbAuthListModel(int order, const wxString &mask);
		virtual ~FbAuthListModel(void);
		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete();
		FbCollection & GetCollection() { return m_collection; }
		void Append(const wxArrayInt &items);
	protected:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); };
		virtual FbModelData * GetData(size_t row);
		static wxString GetOrder(int column);
	private:
        wxString GetSQL(const wxString & order, const wxString & condition);
	private:
		FbCommonDatabase m_database;
		FbCollection m_collection;
		wxArrayInt m_items;
		FbAuthListData * m_data;
		DECLARE_CLASS(FbAuthListModel);
};

#endif // __FBAUTHLIST_H__
