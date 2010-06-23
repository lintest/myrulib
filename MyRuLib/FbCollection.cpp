#include "FbCollection.h"
#include "MyRuLibApp.h"
#include "FbConst.h"
#include "FbGenres.h"
#include "FbDateTime.h"
#include "FbBookTypes.h"

//-----------------------------------------------------------------------------
//  FbCacheBook
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCacheBook, wxObject)

FbCacheBook::FbCacheBook(int code):
	m_code(code),
	m_numb(0),
	m_rate(0),
	m_date(0),
	m_size(0)
{
}

/*
	SELECT DISTINCT
		books.title, books.file_size, books.file_type, books.lang, books.genres, \
		books.md5sum, states.rating, books.created, AGGREGATE(authors.full_name) as full_name \
*/

FbCacheBook::FbCacheBook(int code, wxSQLite3ResultSet &result):
	m_code(code),
	m_name(result.GetString(0)),
	m_auth(result.GetString(8)),
	m_genr(result.GetString(4)),
	m_lang(result.GetString(3)),
	m_type(result.GetString(2)),
	m_md5s(result.GetString(5)),
	m_seqn(wxEmptyString),
	m_numb(0),
	m_rate(result.GetInt(6)),
	m_date(result.GetInt(7)),
	m_size(result.GetInt(1))
{
}

wxString FbCacheBook::GetValue(size_t field)
{
	switch (field) {
		case BF_CODE: return wxString::Format(wxT("%d"), m_code);
		case BF_NAME: return m_name;
		case BF_NUMB: return m_numb ? wxString::Format(wxT("%d"), m_numb) : wxString();
		case BF_AUTH: return m_auth;
		case BF_GENR: return FbGenres::DecodeList(m_genr);
		case BF_RATE: return m_rate ? GetRatingText(m_rate) : wxString();
		case BF_LANG: return m_lang;
		case BF_TYPE: return m_type;
		case BF_DATE: return FbDateTime(m_date).FormatDate();
		case BF_SIZE: return FbCollection::Format(m_size / 1024);
		case BF_BITE: return FbCollection::Format(m_size);
		case BF_SEQN: return m_seqn;
		case BF_MD5S: return m_md5s;
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbCasheBookArray);

//-----------------------------------------------------------------------------
//  FbCacheData
//-----------------------------------------------------------------------------

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
	: m_code(code), m_name(result.GetString(0)), m_count(result.GetInt(1))
{
}

wxString FbCacheData::GetValue(size_t col) const
{
	switch (col) {
		case  0: return m_name;
		case  1: return FbCollection::Format(m_count);
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbCasheDataArray);

//-----------------------------------------------------------------------------
//  FbModelData
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCollection, wxObject)

wxCriticalSection FbCollection::sm_section;

FbCollection::FbCollection(const wxString &filename)
{
	m_database.AttachConfig();
	m_database.CreateFunction(wxT("AGGREGATE"), 1, m_aggregate);
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

wxString FbCollection::GetSeqn(int code, size_t col)
{
	if (code == 0 && col == 0) return _("(Misc.)");

	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	wxString sql = wxT("SELECT value, number FROM sequences WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_seqns, sql);
	return data ? data->GetValue(col) : wxString();
}

wxString FbCollection::GetAuth(int code, size_t col)
{
	if (code == 0 && col == 0) return _("(no Author)");

	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	wxString sql = wxT("SELECT full_name, number FROM authors WHERE id=?");
	FbCacheData * data = collection->GetData(code, collection->m_auths, sql);
	return data ? data->GetValue(col) : wxString();
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

FbCacheData * FbCollection::AddData(FbCasheDataArray &items, FbCacheData * data)
{
	size_t count = items.Count();
	items.Insert(data, 0);
	if (count > DATA_CACHE_SIZE) items.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return data;
}

FbCacheBook * FbCollection::AddBook(FbCacheBook * book)
{
	size_t count = m_books.Count();
	m_books.Insert(book, 0);
	if (count > DATA_CACHE_SIZE) m_books.RemoveAt(DATA_CACHE_SIZE, count - DATA_CACHE_SIZE);
	return book;
}

void FbCollection::ResetSeqn(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->ResetData(collection->m_seqns, code);
}

void FbCollection::ResetAuth(int code)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection) collection->ResetData(collection->m_auths, code);
}

FbCacheData * FbCollection::GetData(int code, FbCasheDataArray &items, const wxString &sql)
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
		if (result.NextRow()) return AddData(items, new FbCacheData(code, result));
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}

void FbCollection::ResetData(FbCasheDataArray &items, int code)
{
	size_t count = items.Count();
	for (size_t i = 0; i < count; i++) {
		if (items[i].GetCode() == code) {
			items.RemoveAt(i);
			break;
		}
	}
}

wxString FbCollection::GetBook(int code, size_t col)
{
	wxCriticalSectionLocker locker(sm_section);
	FbCollection * collection = GetCollection();
	if (collection == NULL) return wxEmptyString;
	FbCacheBook * book = collection->GetCacheBook(code);
	return book ? book->GetValue(col) : wxString();
}

FbCacheBook * FbCollection::GetCacheBook(int code)
{
	size_t count = m_books.Count();
	for (size_t i = 0; i < count; i++) {
		FbCacheBook & book = m_books[i];
		if (book.GetCode() == code) return &book;
	}

	wxString sql = wxT("\
		SELECT DISTINCT \
			books.title, books.file_size, books.file_type, books.lang, books.genres, \
			books.md5sum, states.rating, books.created, AGGREGATE(authors.full_name) as full_name \
		FROM books \
			LEFT JOIN authors ON books.id_author = authors.id \
			LEFT JOIN states ON books.md5sum=states.md5sum \
		WHERE books.id=? \
		GROUP BY books.title, books.file_size, books.file_type, books.lang, states.rating, books.created \
		LIMIT 1 \
	");

	try {
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, code);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return AddBook(new FbCacheBook(code, result));
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return NULL;
}
