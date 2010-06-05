#include "FbCollection.h"

//-----------------------------------------------------------------------------
//  FbCacheData
//-----------------------------------------------------------------------------

#define DATA_CACHE_SIZE 128

IMPLEMENT_CLASS(FbCacheData, wxObject)

FbCacheData::FbCacheData(wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCacheData::FbCacheData(int code, wxSQLite3ResultSet &result)
	: m_code(code), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

wxString FbCacheData::GetValue(size_t col) const
{
	switch (col) {
		case 0:
			return m_name;
		case 1:
			return FbCollection::Format(m_count);
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

wxString FbCollection::Format(int number)
{
	int hi = number / 1000;
	int lo = number % 1000;
	if (hi)
		return Format(hi) << wxChar(0xA0) << wxString::Format(wxT("%03d"), lo);
	else
		return wxString::Format(wxT("%d"), lo);
}

FbCacheData * FbCollection::GetSeqn(int code)
{
	wxString sql = wxT("SELECT id, value, number FROM sequences WHERE id=?");
	return GetData(code, m_seqns, sql);
}

FbCacheData * FbCollection::GetAuth(int code)
{
	wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE id=?");
	return GetData(code, m_auths, sql);
}

void FbCollection::AddSeqn(FbCacheData * data)
{
	AddData(m_seqns, data);
}

void FbCollection::AddAuth(FbCacheData * data)
{
	AddData(m_auths, data);
}

void FbCollection::AddData(FbCasheArray &items, FbCacheData * data)
{
	size_t count = items.Count();
	items.Insert(data, 0);
	if (count > DATA_CACHE_SIZE) items.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
}

FbCacheData * FbCollection::GetData(int code, FbCasheArray &items, const wxString &sql)
{
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		FbCacheData & data = items[i];
		if (data.GetCode() == code) return &data;
	}
	try {
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, code);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) {
			FbCacheData * data = new FbCacheData(code, result);
			AddData(items, data);
			return data;
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

