#ifndef __FBAUTHLIST_H__
#define __FBAUTHLIST_H__

#include "FbTreeModel.h"
#include "FbDatabase.h"
#include "FbCollection.h"

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
		FbAuthListModel(int order, wxChar letter = 0);
		FbAuthListModel(int order, const wxString &mask);
		virtual ~FbAuthListModel(void);
		virtual void Append(FbModelData * data) {}
		virtual void Replace(FbModelData * data) {}
		virtual void Delete();
		FbCollection & GetCollection() { return m_collection; }
	protected:
		virtual size_t GetRowCount() const
			{ return m_items.Count(); }
		virtual FbModelData * GetCurrent()
			{ return GetData(m_position); };
		virtual FbModelData * GetData(size_t row);
	private:
		wxString GetOrder(int column);
        wxString GetSQL(const wxString & order, const wxString & condition);
	private:
		FbCommonDatabase m_database;
		FbCollection m_collection;
		wxArrayInt m_items;
		FbAuthListData * m_data;
		DECLARE_CLASS(FbAuthListModel);
};

#endif // __FBAUTHLIST_H__
