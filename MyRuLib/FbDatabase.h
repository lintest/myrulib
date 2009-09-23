#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

class FbLowerFunction : public wxSQLite3ScalarFunction
{
    virtual void Execute(wxSQLite3FunctionContext& ctx);
};

class FbDatabase: public wxSQLite3Database
{
	public:
        virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
                        int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE);
	private:
		void CreateDatabase();
		void UpgradeDatabase();
    private:
        FbLowerFunction m_lower;
};

class FbAutoCommit
{
    public:
        FbAutoCommit(wxSQLite3Database * database): m_database(database) {
            try {m_database->Begin(WXSQLITE_TRANSACTION_DEFERRED);} catch (...) {};
        };
        virtual ~FbAutoCommit() {
            try {m_database->Commit();} catch (...) {};
        };
    private:
        wxSQLite3Database * m_database;
};

#endif // __FBDATABASE_H__
