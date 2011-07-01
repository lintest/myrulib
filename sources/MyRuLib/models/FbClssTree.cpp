#include "FbClssTree.h"
#include "FbDatabase.h"

//-----------------------------------------------------------------------------
//  FbClssTreeModel
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbClssTreeModel, FbTreeModel)

FbClssTreeModel::FbClssTreeModel(wxSQLite3ResultSet & result)
{
	{
		const wxString file = result.GetString(wxT("dir_file"));
		const wxString data = result.GetString(wxT("dir_data"));
		const wxString type = result.GetString(wxT("dir_type"));
		const wxString code = result.GetString(wxT("dir_code"));
		const wxString name = result.GetString(wxT("dir_name"));
		const wxString info = result.GetString(wxT("dir_info"));
		const wxString prnt = result.GetString(wxT("dir_prnt"));

		wxString sql = wxT("SELECT %s, %s FROM %s WHERE %s=? ORDER BY 2");
		m_ItemSQL = wxString::Format(sql, code.c_str(), name.c_str(), data.c_str(), prnt.c_str());
	}

	{
		const wxString file = result.GetString(wxT("ref_file"));
		const wxString data = result.GetString(wxT("ref_data"));
		const wxString type = result.GetString(wxT("ref_type"));
		const wxString code = result.GetString(wxT("ref_code"));
		const wxString book = result.GetString(wxT("ref_book"));

		wxString sql = wxT("books.%s IN(SELECT %s FROM %s WHERE %s=?)");
		m_ItemSQL = wxString::Format(sql, type.c_str(), book.c_str(), data.c_str(), code.c_str());
	}
}

//-----------------------------------------------------------------------------
//  FbClssModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbClssModelData, FbParentData)

FbClssModelData::FbClssModelData(FbModel & model, FbParentData * parent, wxSQLite3ResultSet & result)
	: FbParentData(model, parent)
	, m_code(result.GetString(0))
	, m_name(result.GetString(1))
	, m_expanded(false) 
	, m_count(0)
{
}

wxString FbClssModelData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case  0: return m_name;
		case  1: return Format(m_count);
		default: return wxEmptyString;
	}
}

bool FbClssModelData::Expand(FbModel & model, bool expand) 
{
	if (m_expanded == expand) return false;

	if (!expand) {
		m_items.Empty();
		return false;
	}

	FbClssTreeModel * tree = wxDynamicCast(&model, FbClssTreeModel);
	if (!tree) return false;

	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(tree->GetItemSQL());
	stmt.Bind(1, m_code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	bool ok = false;
	while (!result.NextRow()) {
		new FbClssModelData(model, this, result);
		ok = true;
	}
	return ok;
}

bool FbClssModelData::operator==(const FbMasterInfo & info) const
{
/*
	FbMasterClssInfo * data = wxDynamicCast(&info, FbMasterClssInfo);
	return data && data->GetId() == m_code;
*/
	return false;
}

