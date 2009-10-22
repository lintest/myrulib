#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

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
	DB_NEW_ZIPFILE = 25,
	DB_WANRAIK_DIR = 26,
	DB_BOOKS_COUNT = 27,
};

class FbLowerFunction : public wxSQLite3ScalarFunction
{
	virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbGenreFunction : public wxSQLite3ScalarFunction
{
	virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbDatabase: public wxSQLite3Database
{
	public:
		virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
						int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
		int NewId(const int iParam);
	protected:
		static const wxString & GetConfigName();
	private:
		static wxCriticalSection sm_queue;
};

class FbAutoCommit
{
	public:
		FbAutoCommit(wxSQLite3Database * database): m_database(database) {
			try {m_database->Begin(WXSQLITE_TRANSACTION_DEFERRED);} catch (...) {};
		};
		FbAutoCommit(wxSQLite3Database & database): m_database(&database) {
			try {m_database->Begin(WXSQLITE_TRANSACTION_DEFERRED);} catch (...) {};
		};
		virtual ~FbAutoCommit() {
			try {m_database->Commit();} catch (...) {};
		};
	private:
		wxSQLite3Database * m_database;
};

class FbCommonDatabase: public FbDatabase
{
	public:
		FbCommonDatabase();
		void AttachConfig();
};

class FbLocalDatabase: public FbDatabase
{
	public:
		FbLocalDatabase();
};

class FbConfigDatabase: public FbDatabase
{
	public:
		void Open();
	private:
		void CreateDatabase();
		void UpgradeDatabase();
		int GetVersion();
		void SetVersion(int iValue);
};

class FbMainDatabase: public FbDatabase
{
	public:
		virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
						int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
	private:
		void CreateDatabase();
		void UpgradeDatabase();
		int GetVersion();
		void SetVersion(int iValue);
};

#endif // __FBDATABASE_H__
