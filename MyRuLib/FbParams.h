#ifndef __FBPARAMS_H__
#define __FBPARAMS_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbDatabase.h"

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
	DB_NEW_ZIPFILE,
	DB_BOOKS_COUNT,
	FB_WANRAIK_DIR,
};

class ParamItem
{
    public:
        ParamItem(int param): id(param), value(0) {};
        ParamItem(wxSQLite3ResultSet & result);
    public:
        int id;
        int value;
        wxString text;
};

WX_DECLARE_OBJARRAY(ParamItem, ParamArray);

class FbParams {
    public:
        FbParams() {};
        static void LoadParams();
        static int GetValue(const int &param);
        static wxString GetText(const int &param);
        void SetValue(const int &param, int value);
        void SetText(const int &param, wxString text);
    private:
        static int DefaultValue(int param);
        static wxString DefaultText(int param);
        static ParamArray sm_params;
    private:
        FbCommonDatabase m_database;
        static wxCriticalSection sm_queue;
};

#endif // __FBPARAMS_H__
