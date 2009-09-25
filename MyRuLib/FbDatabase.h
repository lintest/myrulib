#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

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
        int NewId(int iParam);
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
};

class FbMainDatabase: public FbDatabase
{
	public:
        virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
                        int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
	private:
		void CreateDatabase();
		void UpgradeDatabase();
};

#endif // __FBDATABASE_H__
