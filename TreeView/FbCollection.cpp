#include "FbCollection.h"
#include "FbTreeModel.h"

//-----------------------------------------------------------------------------
//  FbCasheData
//-----------------------------------------------------------------------------

FbCasheData::FbCasheData(int code, wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCasheData::FbCasheData(int code, wxSQLite3ResultSet &result)
	: m_code(code), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

wxString FbAuthListData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 0:
			return m_name;
		case 1:
			return FbModelData::Format(m_code);
		default:
			return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

FbCollection::FbCollection()
{
	//ctor
}

FbCollection::~FbCollection()
{
	//dtor
}
