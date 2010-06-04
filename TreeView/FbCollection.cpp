#include "FbCollection.h"
#include "FbTreeModel.h"

//-----------------------------------------------------------------------------
//  FbCasheData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCasheData, wxObject)

FbCasheData::FbCasheData(wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCasheData::FbCasheData(int code, wxSQLite3ResultSet &result)
	: m_code(code), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

wxString FbCasheData::GetValue(FbModel & model, size_t col) const
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
//  FbColumnArray
//-----------------------------------------------------------------------------

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbCasheArray);

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCollection, wxObject)

FbCollection::FbCollection()
{
	//ctor
}

FbCollection::~FbCollection()
{
	//dtor
}

FbCasheData * FbCollection::GetSeqn(int code)
{
}

FbCasheData * FbCollection::GetAuth(int code)
{

}

void FbCollection::AddSeqn(FbCasheData * data)
{
}

void FbCollection::AddAuth(FbCasheData * data)
{
}
