#ifndef __FBDATABASE_H__
#define __FBDATABASE_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

class FbDatabase: public wxSQLite3Database
{
	public:
        virtual void Open(const wxString& fileName, const wxString& key = wxEmptyString,
                        int flags = WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE);
	private:
		void CreateDatabase();
		void UpgradeDatabase();
};

#endif // __FBDATABASE_H__
