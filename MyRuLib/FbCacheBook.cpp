#include "FbCacheBook.h"
#include "FbColumns.h"
#include "FbGenres.h"
#include "FbConst.h"
#include "FbDateTime.h"
#include "FbCollection.h"

//-----------------------------------------------------------------------------
//  FbCacheBook
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCacheBook, wxObject)

FbCacheBook::FbCacheBook(int code):
	m_code(code),
	m_rate(0),
	m_date(0),
	m_size(0)
{
}

FbCacheBook::FbCacheBook(const FbCacheBook &book):
	m_code(book.m_code),
	m_name(book.m_name),
	m_auth(book.m_auth),
	m_genr(book.m_genr),
	m_lang(book.m_lang),
	m_type(book.m_type),
	m_md5s(book.m_md5s),
	m_rate(book.m_rate),
	m_date(book.m_date),
	m_size(book.m_size)
{
}

FbCacheBook & FbCacheBook::operator =(const FbCacheBook &book)
{
	m_code = book.m_code;
	m_name = book.m_name;
	m_auth = book.m_auth;
	m_genr = book.m_genr;
	m_lang = book.m_lang;
	m_type = book.m_type;
	m_md5s = book.m_md5s;
	m_rate = book.m_rate;
	m_date = book.m_date;
	m_size = book.m_size;
	return *this;
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
	m_rate(result.GetInt(6)),
	m_date(result.GetInt(7)),
	m_size(result.GetInt(1))
{
}

wxString FbCacheBook::GetValue(size_t field) const
{
	switch (field) {
		case BF_CODE: return wxString::Format(wxT("%d"), m_code);
		case BF_NAME: return m_name;
		case BF_NUMB: return wxEmptyString;
		case BF_AUTH: return m_auth;
		case BF_GENR: return FbGenres::DecodeList(m_genr);
		case BF_RATE: return m_rate ? GetRatingText(m_rate) : wxString();
		case BF_LANG: return m_lang;
		case BF_TYPE: return m_type;
		case BF_DATE: return FbDateTime(m_date).FormatDate();
		case BF_SIZE: return FbCollection::Format(m_size / 1024);
		case BF_BITE: return FbCollection::Format(m_size);
		case BF_SEQN: return wxEmptyString;
		case BF_MD5S: return m_md5s;
		default: return wxEmptyString;
	}
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(FbCasheBookArray);

