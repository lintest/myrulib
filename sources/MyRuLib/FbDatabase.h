#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

class FbThread;

wxString Lower(const wxString & input);

wxString Upper(const wxString & input);

wxString & MakeLower(wxString & data);

wxString & MakeUpper(wxString & data);

enum FbDatabaseKey {
	DB_LIBRARY_TITLE = 1,
	DB_LIBRARY_VERSION = 2,
	DB_LIBRARY_TYPE = 3,
	DB_NEW_ARCHIVE = 4,
	DB_NEW_AUTHOR = 5,
	DB_NEW_BOOK = 6,
	DB_NEW_SEQUENCE = 7,
	DB_LIBRARY_DIR = 9,
	DB_LIBRARY_DESCR = 10,
	DB_DOWNLOAD_HOST = 11,
	DB_DOWNLOAD_USER = 12,
	DB_DOWNLOAD_PASS = 13,
	DB_DOWNLOAD_ADDR = 14,
	DB_DATAFILE_DATE = 20,
	DB_DATAFILE_TYPE = 21,
	DB_NEW_ZIPFILE = 25,
	DB_BOOKS_COUNT = 27,
	DB_LAST_BOOK = 30,
	DB_LANG_LIST = 31,
	DB_TYPE_LIST = 32,
};

class FbLowerFunction : public wxSQLite3ScalarFunction
{
	virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbAuthorFunction : public wxSQLite3ScalarFunction
{
	virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbLetterFunction : public wxSQLite3ScalarFunction
{
	virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbIncrementFunction : public wxSQLite3ScalarFunction
{
	public:
		FbIncrementFunction(): m_increment(0) {};
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		int m_increment;
};

class FbAggregateFunction: public wxSQLite3AggregateFunction
{
	public:
		virtual void Aggregate(wxSQLite3FunctionContext& ctx);
		virtual void Finalize(wxSQLite3FunctionContext& ctx);
};

class FbSearchFunction: public wxSQLite3ScalarFunction
{
	public:
		FbSearchFunction(const wxString & input);
		static bool IsFullText(const wxString &text);
		static wxString AddAsterisk(const wxString &text);
	protected:
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		static void Decompose(const wxString &text, wxArrayString &list);
		wxArrayString m_masks;
};

class FbCyrillicCollation: public wxSQLite3Collation
{
	public:
		virtual int Compare(const wxString& text1, const wxString& text2);
};

class FbDatabase: public wxSQLite3Database
{
	public:
		int NewId(const int iParam, int iIncrement = 1);
		wxString GetText(int param);
		void SetText(int param, const wxString & text);
		static const wxString & GetConfigName();
		static wxString GetConfigPath();
		void AttachConfig();
		void JoinThread(FbThread * thread);
	protected:
		static FbCyrillicCollation sm_collation;
	private:
		static wxCriticalSection sm_queue;
};

class FbAutoCommit
{
	public:
		FbAutoCommit(wxSQLite3Database & database): m_database(database)
			{ m_database.Begin(WXSQLITE_TRANSACTION_DEFERRED); }
		virtual ~FbAutoCommit()
			{ m_database.Commit(); }
	private:
		wxSQLite3Database & m_database;
};

class FbCommonDatabase: public FbDatabase
{
	public:
		FbCommonDatabase();
		wxString GetMd5(int id);
};

class FbLocalDatabase: public FbDatabase
{
	public:
		FbLocalDatabase();
};

class FbMasterDatabase: public FbDatabase
{
	protected:
		void UpgradeDatabase(int new_version);
	protected:
		virtual void DoUpgrade(int version) = 0;
		virtual wxString GetMaster() = 0;
	private:
		int GetVersion();
		void SetVersion(int iValue);
};

class FbConfigDatabase: public FbMasterDatabase
{
	public:
		void Open();
	protected:
		virtual void DoUpgrade(int version);
		virtual wxString GetMaster() { return wxT("config"); };
	private:
		void CreateDatabase();
};

class FbMainDatabase: public FbMasterDatabase
{
	public:
		virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
						int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
		void CreateFullText(bool force = false, FbThread * thread = NULL);
	protected:
		virtual void DoUpgrade(int version);
		virtual wxString GetMaster() { return wxT("params"); };
	private:
		void CreateDatabase();
};

#endif // __FBDATABASE_H__
