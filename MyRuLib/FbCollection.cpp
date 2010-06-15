#include "FbCollection.h"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbCacheData
//-----------------------------------------------------------------------------

#define DATA_CACHE_SIZE 128

IMPLEMENT_CLASS(FbCacheData, wxObject)

FbCacheData::FbCacheData(int code, const wxString &name, int count)
	: m_code(code), m_name(name), m_count(count)
{
}

FbCacheData::FbCacheData(wxSQLite3ResultSet &result)
	: m_code(result.GetInt(0)), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCacheData::FbCacheData(int code, wxSQLite3ResultSet &result)
	: m_code(code), m_name(result.GetString(1)), m_count(result.GetInt(2))
{
}

FbCacheData::FbCacheData(const FbCacheData &data)
	: m_code(data.m_code), m_name(data.m_name), m_count(data.m_count)
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

wxCriticalSection FbCollection::sm_section;

FbCollection::FbCollection(const wxString &filename)
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

FbCollection * FbCollection::GetCollection()
{
	return wxGetApp().GetCollection();
}

FbCacheData FbCollection::GetSeqn(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return NULL;
	wxString sql = wxT("SELECT id, value, number FROM sequences WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_seqns, sql);
	return data ? *data : FbCacheData(0);
}

FbCacheData FbCollection::GetAuth(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return NULL;
	wxString sql = wxT("SELECT id, full_name, number FROM authors WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_auths, sql);
	return data ? *data : FbCacheData(0);
}

void FbCollection::AddSeqn(FbCacheData * data)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddData(collection->m_seqns, data);
}

void FbCollection::AddAuth(FbCacheData * data)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->AddData(collection->m_auths, data);
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

