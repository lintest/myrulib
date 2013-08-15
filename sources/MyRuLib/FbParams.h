#ifndef __FBPARAMS_H__
#define __FBPARAMS_H__

#include <wx/wx.h>
#include <wx/arrimpl.cpp>
#include "wx/FbSQLite3.h"
#include "FbDatabase.h"


#define FB_FRAME_OFFSET 1000

/// WARNING !!!
///
/// FbParamKey < FB_FRAME_OFFSET
/// FbFrameKey < FB_FRAME_OFFSET

enum FbParamKey {
	FB_CONFIG_TITLE   = 1,
	FB_CONFIG_VERSION = 2,
	FB_CONFIG_TYPE    = 3,

	FB_NEW_FOLDER	 = 101,
	FB_NEW_COMMENT	 = 102,
	FB_NEW_DOWNLOAD	 = 103,
	FB_NEW_SCRIPT	 = 104,

	FB_RECENT_0 = 110,
	FB_RECENT_1 = 111,
	FB_RECENT_2 = 112,
	FB_RECENT_3 = 113,
	FB_RECENT_4 = 114,
	FB_RECENT_5 = 115,

	FB_WINE_DIR        = 117,
	FB_TEMP_DIR        = 118,
	FB_TEMP_DEL        = 119,
	FB_REMOVE_FILES    = 120,
	FB_EXTERNAL_DIR	   = 121,
	FB_TRANSLIT_FOLDER = 122,
	FB_TRANSLIT_FILE   = 123,
	FB_FOLDER_FORMAT   = 124,
	FB_FILE_FORMAT	   = 125,
	FB_SAVE_FULLPATH   = 126,
	FB_USE_UNDERSCORE  = 127,

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
	FB_FILTER_DEL  = 145,

	FB_AUTO_DOWNLD  = 150,
	FB_USE_PROXY	= 151,
	FB_PROXY_ADDR	= 152,
	FB_PROXY_USER   = 153,
	FB_DOWNLOAD_DIR = 154,
	FB_DEL_DOWNLOAD = 155,
	FB_HTTP_IMAGES  = 156,
	FB_WEB_TIMEOUT  = 157,
	FB_WEB_ATTEMPT  = 158,
	FB_PROXY_PASS   = 159,

	FB_FONT_MAIN = 160,
	FB_FONT_HTML = 161,
	FB_FONT_TOOL = 162,
	FB_FONT_DLG  = 163,

	FB_COLOUR_MAIN = 165,
	FB_COLOUR_HTML = 166,
	FB_COLOUR_TOOL = 167,
	FB_COLOUR_DLG  = 168,

	FB_FRAME_MAXIMIZE = 170,
	FB_FRAME_WIDTH    = 171,
	FB_FRAME_HEIGHT   = 172,
	FB_FRAME_LIST     = 173,

	FB_COLUMN_TYPE   = 180,
	FB_COLUMN_SYZE   = 181,
	FB_COLUMN_GENRE  = 182,
	FB_COLUMN_RATING = 183,
	FB_COLUMN_LANG   = 184,

	FB_LAST_LETTER   = 190,
	FB_NOTEBOOK_ART  = 191,
	FB_ALPHABET_RU   = 192,
	FB_ALPHABET_EN   = 193,
	FB_LANG_LOCALE   = 194,
	FB_IMAGE_WIDTH   = 195,
	FB_STATUS_SHOW   = 196,
	FB_USE_COOLREADER = 197,
	FB_FILE_LENGTH   = 198,

	FB_GRAY_FONT     = 211,
	FB_CLEAR_LOG     = 212,
	FB_NUMBER_FORMAT = 213,
	FB_GRID_HRULES   = 214,
	FB_GRID_VRULES   = 215,

	FB_TITLE_0 = 220,
	FB_TITLE_1 = 221,
	FB_TITLE_2 = 222,
	FB_TITLE_3 = 223,
	FB_TITLE_4 = 224,
	FB_TITLE_5 = 225,

	FB_READER_FONT_COLOUR  = 901,
	FB_READER_BACK_COLOUR  = 902,
	FB_READER_FONT_NAME    = 903,
	FB_READER_FONT_SIZE    = 904,
	FB_HEADER_FONT_NAME    = 905,
	FB_HEADER_FONT_SIZE    = 906,
	FB_HEADER_FONT_COLOUR  = 907,
	FB_READER_SHOW_HEADER  = 908,
	FB_READER_INTERLINE    = 909,
	FB_READER_HYPHENATION  = 910,
	FB_READER_PAGE_COUNT   = 911,
};

enum FbFrameKey {
	FB_VIEW_MODE = 0,
	FB_LIST_MODE,
	FB_BOOK_COLUMNS,
};

class FbParamItem {
	public:
		FbParamItem(const FbParamItem & item): m_param(item.m_param) {}
		FbParamItem(int param): m_param(param) {}
		FbParamItem(wxWindowID winid, int param): m_param(Param(winid, param)) {}
		operator int () const;
		operator wxFont () const;
		operator wxString () const;
		FbParamItem & operator = (int value);
		FbParamItem & operator = (bool value);
		FbParamItem & operator = (const wxString & value);
		int Int() const;
		wxString Str() const;
	public:
		static int GetInt(wxWindowID winid, int param);
		static wxString GetPath(int param);
		static wxString GetStr(wxWindowID winid, int param);
		static wxFont GetFont(int param);
		static wxColour GetColour(int param);
		static void Set(int param, int value);
		static void Set(wxWindowID winid, int param, int value);
		static void Set(int param, const wxString &text);
		static void Set(wxWindowID winid, int param, const wxString &text);
		static void Reset(int param);
		static int DefaultInt(int param);
		static wxString DefaultStr(int param);
		static void AddRecent(const wxString &text, const wxString &title);
		static bool IsGenesis();
	private:
		static wxString GetStr(int param);
		static int GetInt(int param);
		static int Param(wxWindowID winid, int param);
		const int m_param;
};

class FbParamList {
	public:
		FbParamList() {}
		FbParamItem operator()(int param)
			{ return FbParamItem(param); }
		FbParamItem operator()(wxWindowID winid, int param)
			{ return FbParamItem(winid, param); }
	public:
		long Style(long style = wxBORDER_SUNKEN) const;
};

extern FbParamList FbParams;

#endif // __FBPARAMS_H__
