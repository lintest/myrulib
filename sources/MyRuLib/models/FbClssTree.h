#ifndef __FBCLSSLIST_H__
#define __FBCLSSLIST_H__

#include "controls/FbTreeModel.h"
#include "FbMasterTypes.h"

class FbClssTreeModel: public FbTreeModel
{
	public:
		FbClssTreeModel(wxSQLite3ResultSet & result);
		wxString GetItemSQL() const { return m_ItemSQL; }
		wxString GetBookSQL() const { return m_BookSQL; }
	private:
		wxString m_ItemSQL;
		wxString m_BookSQL;
	private:
		DECLARE_CLASS(FbClssTreeModel);
};

class FbClssModelData: public FbParentData
{
	public:
		FbClssModelData(FbModel & model, const wxString & name = wxEmptyString);
		FbClssModelData(FbModel & model, FbParentData * parent, wxSQLite3ResultSet & result);
		void SetCount(int count) { m_count = count; }
		const wxString & GetCode() const { return m_code; }
	public:
		virtual wxString GetValue(FbModel & model, size_t col) const;
		virtual bool IsExpanded(FbModel & model) const { return m_expanded; }
		virtual bool Expand(FbModel & model, bool expand);
		virtual bool HiddenRoot() const { return false; }
		virtual FbMasterInfo GetInfo(FbModel & model) const;
		bool operator==(const FbMasterInfo & info) const;
	protected:
		const wxString m_code;
		wxString m_name;
		int m_count;
		bool m_expanded;
	private:
		DECLARE_CLASS(FbClssModelData);
};

#endif // __FBCLSSLIST_H__
