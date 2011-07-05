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

		wxString sql = wxT("SELECT DISTINCT a.%s, a.%s, b.%s FROM %s a LEFT JOIN %s b ON a.%s=b.%s WHERE a.%s=? ORDER BY 2");
		m_ItemSQL = wxString::Format(sql, code.c_str(), name.c_str(), prnt.c_str(), data.c_str(), data.c_str(), code.c_str(), prnt.c_str(), prnt.c_str());
	}

	{
		const wxString file = result.GetString(wxT("ref_file"));
		const wxString data = result.GetString(wxT("ref_data"));
		const wxString type = result.GetString(wxT("ref_type"));
		const wxString code = result.GetString(wxT("ref_code"));
		const wxString book = result.GetString(wxT("ref_book"));

		wxString sql = wxT("books.%s IN(SELECT %s FROM %s WHERE %s=?)");
		m_BookSQL = wxString::Format(sql, type.c_str(), book.c_str(), data.c_str(), code.c_str());
	}
}

//-----------------------------------------------------------------------------
//  FbClssModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbClssModelData, FbParentData)

FbClssModelData::FbClssModelData(FbModel & model, const wxString & name)
	: FbParentData(model, NULL)
	, m_code(wxT("0"))
	, m_name(name)
	, m_children(false)
	, m_expanded(false) 
	, m_count(0)
{
}

FbClssModelData::FbClssModelData(FbModel & model, FbParentData * parent, wxSQLite3ResultSet & result)
	: FbParentData(model, parent)
	, m_code(result.GetString(0))
	, m_name(result.GetString(1))
	, m_children(result.GetInt(2))
	, m_expanded(false) 
	, m_count(0)
{
}

wxString FbClssModelData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case  0: return m_name;
		case  1: return m_count ? Format(m_count) : wxString();
		default: return wxEmptyString;
	}
}

bool FbClssModelData::Expand(FbModel & model, bool expand) 
{
	if (m_expanded == expand) return false;
	
	m_expanded = expand;

	if (!expand) {
		m_items.Empty();
		return true;
	}

	FbClssTreeModel * tree = wxDynamicCast(&model, FbClssTreeModel);
	if (!tree) return false;

	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(tree->GetItemSQL());
	stmt.Bind(1, m_code);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		new FbClssModelData(model, this, result);
	}
	return true;
}

bool FbClssModelData::operator==(const FbMasterInfo & info) const
{
/*
	FbMasterClssInfo * data = wxDynamicCast(&info, FbMasterClssInfo);
	return data && data->GetId() == m_code;
*/
	return false;
}

