#ifndef __FBAUTHLISTMODEL_H__
#define __FBAUTHLISTMODEL_H__

#include "FbTreeModel.h"
#include <wx/wxsqlite3.h>

#define AUTHOR_CASHE_SIZE 100

class FbAuthListModelData 
{
	public:
		FbAuthListModelData(unsigned int id): m_rowid(id), m_auth(0) {} 
		FbAuthListModelData(unsigned int id, wxSQLite3ResultSet &result);
		wxString GetValue(size_t col);
		int GetId() { return m_rowid; }
		int GetAuth() { return m_auth; }
	private:
		unsigned int m_rowid;
		int m_auth;
		wxString m_name;
		int m_count;
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbAuthListModelData, FbAuthListModelArray);

class FbAuthListModel: public FbTreeModelList
{
	public:
		FbAuthListModel(const wxString &filename, const wxString &order, const wxString &letter = wxEmptyString);
		virtual ~FbAuthListModel(void);
		virtual wxString GetValue(const FbTreeItemId &id, size_t col);
		virtual FbTreeItemId GetCurrent();
	private:
        FbAuthListModelData * FindRow(const FbTreeItemId &id);
        FbAuthListModelData * FindRow(size_t rowid);
	private:
		wxSQLite3Database m_database;
		FbAuthListModelArray m_data;
};

#endif // __FBAUTHLISTMODEL_H__