#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>
#include "FbDatabase.h"
#include "FbBookInfo.h"

class FbModel;

#define DATA_CACHE_SIZE 128
#define HTML_CACHE_SIZE  16

class FbCacheBook: public wxObject
{
	public:
		FbCacheBook(int code = 0);
		FbCacheBook(int code, wxSQLite3ResultSet &result);
		FbCacheBook(const FbCacheBook &book);
		FbCacheBook & operator =(const FbCacheBook &book);
		operator bool() const { return m_code; }
		int GetCode() const { return m_code; }
		wxString GetValue(size_t field);
	private:
		int m_code;
		wxString m_name;
		wxString m_auth;
		wxString m_genr;
		wxString m_lang;
		wxString m_type;
		wxString m_md5s;
		int m_rate;
		int m_date;
		int m_size;
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
		static wxString GetBook(int code, size_t col);
		static wxString GetBookHTML(int code, const wxString &md5sum, bool bVertical, bool bEditable, const wxString &filetype);
		FbCacheBook GetBookData(int code);
		static void AddSeqn(FbCacheData * data);
		static void AddAuth(FbCacheData * data);
		static void AddInfo(FbBookInfo * info);
		static void ResetSeqn(int code);
		static void ResetAuth(int code);
	protected:
		FbCacheData * GetData(int code, FbCasheDataArray &items, const wxString &sql);
		FbCacheData * AddData(FbCasheDataArray &items, FbCacheData * data);
		FbCacheBook * AddBook(FbCacheBook * book);
		void AddBook(FbBookInfo * info);
		void ResetData(FbCasheDataArray &items, int code);
		FbCacheBook * GetCacheBook(int code);
		FbBookInfo * GetCacheInfo(int code);
	private:
		static wxCriticalSection sm_section;
		FbCommonDatabase m_database;
		FbAggregateFunction m_aggregate;
		FbCasheDataArray m_auths;
		FbCasheDataArray m_seqns;
		FbCasheBookArray m_books;
		FbBookInfoArray m_infos;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
