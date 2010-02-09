#ifndef __FBPARAMS_H__
#define __FBPARAMS_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbDatabase.h"

enum FbParamKey {
	FB_CONFIG_TITLE   = 1,
	FB_CONFIG_VERSION = 2,
	FB_CONFIG_TYPE    = 3,

	FB_NEW_FOLDER	  = 101,
	FB_NEW_COMMENT	 = 102,
	FB_NEW_DOWNLOAD	= 103,

	FB_RECENT_0 = 110,
	FB_RECENT_1 = 111,
	FB_RECENT_2 = 112,
	FB_RECENT_3 = 113,
	FB_RECENT_4 = 114,
	FB_RECENT_5 = 115,

	FB_TEMP_DIR        = 118,
	FB_TEMP_DEL        = 119,
	FB_REMOVE_FILES    = 120,
	FB_EXTERNAL_DIR	   = 121,
	FB_TRANSLIT_FOLDER = 122,
	FB_TRANSLIT_FILE   = 123,
	FB_FOLDER_FORMAT   = 124,
	FB_FILE_FORMAT	   = 125,
	FB_SAVE_FULLPATH   = 126,

	FB_VIEW_SEQUEN   = 128,
	FB_MODE_SEQUEN   = 129,
	FB_VIEW_AUTHOR	 = 130,
	FB_VIEW_GENRES	 = 131,
	FB_VIEW_SEARCH	 = 132,
	FB_VIEW_FOLDER	 = 133,
	FB_VIEW_DOWNLD	 = 134,
	FB_MODE_AUTHOR	 = 135,
	FB_MODE_GENRES	 = 136,
	FB_MODE_SEARCH	 = 137,
	FB_MODE_FOLDER	 = 138,
	FB_MODE_DOWNLD	 = 139,

	FB_USE_FILTER  = 140,
	FB_FILTER_LIB  = 141,
	FB_FILTER_USR  = 142,
	FB_FILTER_LANG = 143,
	FB_FILTER_TYPE = 144,

	FB_LIBRUSEC_URL = 150,
	FB_USE_PROXY	= 151,
	FB_PROXY_ADDR	= 152,
	FB_AUTO_DOWNLD  = 153,
	FB_DOWNLOAD_DIR = 154,
	FB_DEL_DOWNLOAD = 155,
	FB_HTTP_IMAGES  = 156,

	FB_FONT_MAIN = 160,
	FB_FONT_HTML = 161,
	FB_FONT_TOOL = 162,
	FB_FONT_DLG  = 163,

	FB_FRAME_MAXIMIZE = 170,
	FB_FRAME_WIDTH    = 171,
	FB_FRAME_HEIGHT   = 172,

	FB_COLUMN_TYPE   = 180,
	FB_COLUMN_SYZE   = 181,
	FB_COLUMN_GENRE  = 182,
	FB_COLUMN_RATING = 183,
	FB_COLUMN_LANG   = 184,

	FB_LAST_LETTER   = 190,
	FB_NOTEBOOK_ART  = 191,
	FB_ALPHABET_RU   = 192,
	FB_ALPHABET_EN   = 193,

	FB_LIMIT_CHECK   = 210,
	FB_LIMIT_COUNT   = 211,

	FB_TITLE_0 = 220,
	FB_TITLE_1 = 221,
	FB_TITLE_2 = 222,
	FB_TITLE_3 = 223,
	FB_TITLE_4 = 224,
	FB_TITLE_5 = 225,
};

class ParamItem
{
	public:
		ParamItem(const int param): id(param), value(0) {};
		ParamItem(wxSQLite3ResultSet & result);
	public:
		int id;
		int value;
		wxString text;
};

WX_DECLARE_OBJARRAY(ParamItem, ParamArray);

class FbParams {
	public:
		FbParams();
		void LoadParams();
		static int GetValue(const int param);
		static wxString GetText(const int param);
		static wxFont GetFont(const int param);
		void SetValue(const int param, int value);
		void SetText(const int param, const wxString &text);
		void ResetValue(const int param);
		static int DefaultValue(int param);
		static wxString DefaultText(int param);
		static wxString GetLimit();
		void AddRecent(const wxString &text, const wxString &title);
	private:
		static ParamArray sm_params;
		FbCommonDatabase m_database;
		static wxCriticalSection sm_queue;
};

#endif // __FBPARAMS_H__
