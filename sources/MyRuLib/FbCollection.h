#ifndef __FBCOLLECTION_H__
#define __FBCOLLECTION_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include "FbDatabase.h"
#include "FbViewData.h"
#include "FbCacheBook.h"
#include "FbThread.h"

class FbModel;

#define DATA_CACHE_SIZE 128
#define HTML_CACHE_SIZE  16

class FbBookAuths: public wxObject
{
	public:
		FbBookAuths(int code, FbDatabase & database);
		int GetCode() const { return m_code; }
		wxString operator[](size_t col) const;
	private:
		int m_code;
		wxString m_name;
		DECLARE_CLASS(FbBookAuths)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbBookAuths, FbBookAuthsArray);

class FbBookSeqns: public wxObject
{
	public:
		FbBookSeqns(int code, wxSQLite3Database &database);
		int GetCode() const { return m_code; }
		wxString operator[](size_t col) const;
	private:
		int m_code;
		wxString m_name;
		int m_numb;
		DECLARE_CLASS(FbBookSeqns)
};

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(FbBookSeqns, FbBookSeqnsArray);

class FbParamData: public wxObject
{
	public:
		FbParamData(int val = 0, const wxString &str = wxEmptyString)
			: m_int(val), m_str(str) {}
		FbParamData & operator=(wxSQLite3ResultSet & result);
	private:
		int m_int;
		wxString m_str;
		friend class FbCollection;
		DECLARE_CLASS(FbParamData)
};

#include <wx/hashmap.h>
WX_DECLARE_HASH_MAP( int, FbParamData, wxIntegerHash, wxIntegerEqual, FbParamHash);
WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, FbNameHash);

class FbCollection: public wxObject
{
	public:
		static void LoadConfig();
		static wxString Format(int number);
		FbCollection(const wxString &filename);
		virtual ~FbCollection();
		bool IsOk() const;
	public:
		static FbCollection * GetCollection();
		static wxString GetSeqn(int code, size_t col);
		static wxString GetAuth(int code, size_t col);
		static wxString GetBook(int code, size_t col);
		static wxString GetBookHTML(const FbViewContext &ctx, const FbCacheBook &book, int code);
		static FbCacheBook GetBookData(int code);
		static void AddSeqn(int code, const wxString &name);
		static void AddAuth(int code, const wxString &name);
		static void AddInfo(FbViewData * info);
		static void ResetSeqn(int code);
		static void ResetAuth(int code);
		static void ResetInfo(int code);
		static void ResetBook(int code);
		static void ResetBook(const wxArrayInt &books);
		static void LoadIcon(int book);
		static void AddIcon(wxString extension, wxBitmap bitmap);
		static wxString GetIcon(const wxString &extension);
		static void EmptyInfo();
		static int GetParamInt(int param);
		static wxString GetParamStr(int param);
		static void SetParamInt(int param, int value);
		static void SetParamStr(int param, const wxString &value);
		static void ResetParam(int param);
		static void GetDown(wxArrayInt &items);
		static void DownMove(int code, int direction);
		static void DownDelete(int code);
		static size_t DownCount();
		static int GetDown(size_t index);
	protected:
//		FbCacheData * GetData(int code, FbCasheDataArray &items, const wxString &sql);
//		FbCacheData * AddData(FbCasheDataArray &items, FbCacheData * data);
		FbCacheBook AddBook(const FbCacheBook &book);
		void AddBookInfo(FbViewData * info);
		void DoResetBook(const wxArrayInt &books);
		void DoResetBook(int code);
		void DoResetInfo(int code);
		FbCacheBook GetCacheBook(int code);
		FbViewData * GetCacheInfo(int code);
		wxString GetBookAuths(int code, size_t col);
		wxString GetBookSeqns(int code, size_t col);
		void LoadParams();
		int DefaultInt(int param);
		wxString DefaultStr(int param);
		bool IsGenesis() const;
	private:
		static wxCriticalSection sm_section;
		static wxArrayString sm_icons;
		static wxArrayString sm_noico;
		static FbParamHash sm_params;
		FbMainDatabase m_database;
		FbAggregateFunction m_aggregate;
		FbNameHash m_auths;
		FbNameHash m_seqns;
		FbCasheBookArray m_books;
		FbBookAuthsArray m_book_auth;
		FbBookSeqnsArray m_book_seqn;
		FbViewDataArray m_infos;
		FbParamHash m_params;
		FbThread * m_thread;
		wxArrayInt * m_downs;
		DECLARE_CLASS(FbCollection)
};

#endif // __FBCOLLECTION_H__
