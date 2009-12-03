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

class FbSearchFunction: public wxSQLite3ScalarFunction
{
	public:
		FbSearchFunction(const wxString & input);
	protected:
		virtual void Execute(wxSQLite3FunctionContext& ctx);
	private:
		void Decompose(const wxString &text, wxArrayString &list);
		wxString Lower(const wxString & text);
		wxArrayString m_masks;
};

class FbDatabase: public wxSQLite3Database
{
	public:
		virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
						int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
		int NewId(const int iParam, int iIncrement = 1);
		wxString GetText(const int param);
		static const wxString & GetConfigName();
	private:
		static wxCriticalSection sm_queue;
};

class FbAutoCommit
{
	public:
		FbAutoCommit(wxSQLite3Database & database): m_database(database) {
			try {m_database.Begin(WXSQLITE_TRANSACTION_DEFERRED);} catch (...) {};
		};
		virtual ~FbAutoCommit() {
			try {m_database.Commit();} catch (...) {};
		};
	private:
		wxSQLite3Database & m_database;
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
	protected:
		virtual void DoUpgrade(int version);
		virtual wxString GetMaster() { return wxT("params"); };
	private:
		void CreateDatabase();
};

#endif // __FBDATABASE_H__
