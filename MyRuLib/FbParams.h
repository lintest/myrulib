#ifndef __FBPARAMS_H__
#define __FBPARAMS_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbDatabase.h"

enum FbParamKey {
	FB_CONFIG_TITLE = 1,
	FB_CONFIG_VERSION = 2,
	FB_NEW_FOLDER = 3,

	FB_LIBRARY_DIR = 10,
	FB_WANRAIK_DIR = 11,
	FB_DOWNLOAD_DIR = 12,
	FB_EXTERNAL_DIR = 13,
	FB_TRANSLIT_FOLDER = 14,
	FB_TRANSLIT_FILE = 15,
	FB_FOLDER_FORMAT = 16,
	FB_FILE_FORMAT = 17,

	FB_VIEW_TYPE = 21,
	FB_LIST_TYPE = 22,
	FB_FB2_ONLY = 23,

    FB_USE_PROXY = 30,
    FB_PROXY_ADDR = 31,
    FB_PROXY_PORT = 32,
    FB_PROXY_NAME = 33,
    FB_PROXY_PASS = 34,
};

class ParamItem
{
    public:
        ParamItem(FbParamKey param): id(param), value(0) {};
        ParamItem(wxSQLite3ResultSet & result);
    public:
        FbParamKey id;
        int value;
        wxString text;
};

WX_DECLARE_OBJARRAY(ParamItem, ParamArray);

class FbParams {
    public:
        FbParams();
//        FbParams(FbDatabase * database): m_database(database) {};
        static int GetValue(const FbParamKey param);
        static wxString GetText(const FbParamKey param);
        void LoadParams();
        void SetValue(const FbParamKey param, int value);
        void SetText(const FbParamKey param, wxString text);
    private:
        static int DefaultValue(FbParamKey param);
        static wxString DefaultText(FbParamKey param);
        static ParamArray sm_params;
    private:
        FbDatabase * m_database;
        static wxCriticalSection sm_queue;
};

#endif // __FBPARAMS_H__
