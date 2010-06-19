#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>
#include "FbDatabase.h"

class FbModel;

#define DATA_CACHE_SIZE 128

typedef size_t FbBookFields;
#define	BF_CODE  0x0001
#define	BF_NAME  0x0002
#define	BF_NUMB  0x0004
#define	BF_AUTH  0x0008
#define	BF_GENR  0x0010
#define	BF_RATE  0x0020
#define	BF_LANG  0x0040
#define	BF_TYPE  0x0080
#define	BF_DATE  0x0100
#define	BF_SIZE  0x0200
#define	BF_SEQN  0x0400
#define	BF_MD5S  0x0800

/*
enum FbBookFields {
	BF_CODE = 0x0001,
	BF_NAME = 0x0002,
	BF_NUMB = 0x0004,
	BF_AUTH = 0x0008,
	BF_GENR = 0x0010,
	BF_RATE = 0x0020,
	BF_LANG = 0x0040,
	BF_TYPE = 0x0080,
	BF_DATE = 0x0100,
	BF_SIZE = 0x0200,
	BF_SEQN = 0x0400,
	BF_MD5S = 0x0800,
};
*/
class FbCacheBook: public wxObject
{
	public:
		FbCacheBook(int code = 0);
		FbCacheBook(int code, wxSQLite3ResultSet &result);
		FbCacheBook(const FbCacheBook &book);
		int GetCode() const { return m_code; }
		int GetFields() const { return m_fields; }
		wxString GetValue(FbBookFields field);
		bool HasField(size_t col) const;
	private:
		int m_code;
		wxString m_name;
		wxString m_auth;
		wxString m_genr;
		wxString m_lang;
		wxString m_type;
		wxString m_md5s;
		wxString m_seqn;
		int m_numb;
		int m_rate;
		int m_date;
		int m_size;
		long m_fields;
		DECLARE_CLASS(FbCacheBook)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCacheBook, FbCasheBookArray);

class FbCacheData: public wxObject
{
	public:
		FbCacheData(wxSQLite3ResultSet &result);
		FbCacheData(int code, wxSQLite3ResultSet &result);
		FbCacheData(int code, const wxString &name = wxEmptyString, int count = 0);
		int GetCode() const { return m_code; }
		wxString GetValue(size_t col) const;
	private:
		int m_code;
		wxString m_name;
		int m_count;
		DECLARE_CLASS(FbCacheData)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbCacheData, FbCasheDataArray);

class FbCollection: public wxObject
{
	public:
		static wxString Format(int number);
		FbCollection(const wxString &filename);
		virtual ~FbCollection();
	public:
		static FbCollection * GetCollection();
		static wxString GetSeqn(int code, size_t col);
		static wxString GetAuth(int code, size_t col);
		static FbCacheBook GetBook(int code);
		static void AddSeqn(FbCacheData * data);
		static void AddAuth(FbCacheData * data);
		static void ResetSeqn(int code);
		static void ResetAuth(int code);
	protected:
		FbCacheData * GetData(int code, FbCasheDataArray &items, const wxString &sql);
		FbCacheData * AddData(FbCasheDataArray &items, FbCacheData * data);
		FbCacheBook * AddBook(FbCacheBook * book);
		void ResetData(FbCasheDataArray &items, int code);
		FbCacheBook * GetCacheBook(int code);
	private:
		static wxCriticalSection sm_section;
		FbCommonDatabase m_database;
		FbAggregateFunction m_aggregate;
		FbCasheDataArray m_auths;
		FbCasheDataArray m_seqns;
		FbCasheBookArray m_books;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
