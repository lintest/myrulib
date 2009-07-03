#ifndef __FBPARAMS_H__
#define __FBPARAMS_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <DatabaseLayer.h>
#include "db/Params.h"

enum {
	DB_LIBRARY_TITLE = 1,
	DB_LIBRARY_VERSION = 2,
	DB_LIBRARY_TYPE,
	DB_NEW_ARCHIVE,
	DB_NEW_AUTHOR,
	DB_NEW_BOOK,
	DB_NEW_SEQUENCE,
	FB_FB2_PROGRAM,
	FB_LIBRARY_DIR,
	FB_DOWNLOAD_DIR,
	FB_EXTRACT_DIR,
	FB_EXTRACT_DELETE,
	FB_EXTERNAL_DIR,
	FB_TRANSLIT_FOLDER,
	FB_TRANSLIT_FILE,
	FB_FOLDER_FORMAT,
	FB_FILE_FORMAT,
    FB_USE_PROXY,
    FB_PROXY_ADDR,
    FB_PROXY_PORT,
    FB_PROXY_NAME,
    FB_PROXY_PASS,
	FB_VIEW_TYPE,
	FB_FB2_ONLY,
};

class ParamItem
{
    public:
        ParamItem(int param): id(param) {};
        ParamItem(ParamsRow * row): id(row->id), value(row->value), text(row->text) {};
    public:
        int id;
        int value;
        wxString text;
};

WX_DECLARE_OBJARRAY(ParamItem, ParamArray);

class FbParams {
    public:
        static void InitParams(DatabaseLayer *database)
        {
            database->RunQuery(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
            database->RunQuery(_("INSERT INTO params(id, text)  VALUES (1, 'Test Library');"));
            database->RunQuery(_("INSERT INTO params(id, value) VALUES (2, 1);"));
        };
    private:
        DatabaseLayer *m_database;
        int DefaultValue(int param);
        wxString DefaultText(int param);
        static ParamArray & GetParams();
    public:
        FbParams();
        FbParams(DatabaseLayer *database, wxCriticalSection &section);
        static void LoadParams();
        int GetValue(const int &param);
        wxString GetText(const int &param);
        void SetValue(const int &param, int value);
        void SetText(const int &param, wxString text);
};

#endif // __FBPARAMS_H__
