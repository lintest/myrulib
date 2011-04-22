#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbBookData.h"
#include "FbCollection.h"
#include "FbConst.h"

int FbParams::GetInt(int param)
{
	return FbCollection::GetParamInt(param);
};

wxString FbParams::GetStr(int param)
{
	return FbCollection::GetParamStr(param);
};

void FbParams::Set(int param, int value)
{
	return FbCollection::SetParamInt(param, value);
}

void FbParams::Set(int param, const wxString &text)
{
	return FbCollection::SetParamStr(param, text);
}

int FbParams::DefaultInt(int param)
{
	if (param < FB_FRAME_OFFSET)
		switch (param) {
			case FB_TEMP_DEL: return 1;
			case FB_GRAY_FONT: return 1;
			case FB_MODE_AUTHOR: return 1;
			case FB_TRANSLIT_FOLDER: return 0;
			case FB_TRANSLIT_FILE: return 1;
			case FB_USE_PROXY: return 0;
			case FB_HTTP_IMAGES: return 0;
			case FB_AUTO_DOWNLD: return 1;
			case FB_FRAME_WIDTH: return 640;
			case FB_FRAME_HEIGHT: return 480;
			case FB_ALPHABET_RU: return 1;
			case FB_ALPHABET_EN: return 1;
			case FB_WEB_TIMEOUT: return 600;
			case FB_WEB_ATTEMPT: return 10;
			case FB_IMAGE_WIDTH: return 200;
			case FB_LANG_LOCALE: return wxLANGUAGE_DEFAULT;
			default: return 0;
		}
	else {
		switch (param) {
			case (FB_FRAME_OFFSET + FB_LIST_MODE): return 1;
		}
		switch (param % FB_FRAME_OFFSET) {
			default: return 0;
		}
	}
};

wxString FbParams::DefaultStr(int param)
{
	if (param < FB_FRAME_OFFSET)
		switch (param) {
			case DB_LIBRARY_DIR:
				return wxT('.');
			case DB_DOWNLOAD_HOST:
				return wxT("flibusta.net");
			case DB_DOWNLOAD_ADDR:
				return wxT("http://%h/b/%i/download");
			case FB_DOWNLOAD_DIR:
				return FbDatabase::GetConfigPath() + wxFileName::GetPathSeparator() + wxT("download");
			case FB_TEMP_DIR:
				return FbDatabase::GetConfigPath() + wxFileName::GetPathSeparator() + wxT("local");
			case FB_EXTERNAL_DIR:
				return FbDatabase::GetConfigPath() + wxFileName::GetPathSeparator() + wxT("export");
			case FB_FONT_MAIN:
			case FB_FONT_HTML:
			case FB_FONT_TOOL:
			case FB_FONT_DLG:
				return wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT).GetNativeFontInfoDesc();
			case FB_FRAME_LIST:
				return wxT('0');
			case FB_FOLDER_FORMAT:
				return wxT("%a/%f/%s/%n %t");
			default:
				return wxEmptyString;
		}
	else {
		switch (param) {
			case (FB_FRAME_OFFSET + FB_BOOK_COLUMNS):
				return wxT("CEFH");
			case (FB_FRAME_OFFSET * 3 + FB_BOOK_COLUMNS):
				return wxT("AEFH");
			case (FB_FRAME_OFFSET * 5 + FB_BOOK_COLUMNS):
				return wxT("ALEFH");
		}
		switch (param % FB_FRAME_OFFSET) {
			case FB_BOOK_COLUMNS:
				return wxT("ACEFH");
			default:
				return wxEmptyString;
		}
	}
};

wxFont FbParams::GetFont(int param)
{
	wxString info = GetStr(param);
	if (info.IsEmpty()) return wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font;
	font.SetNativeFontInfo(info);
	return font;
}

wxColour FbParams::GetColour(int param)
{
	return wxColour(GetStr(param));
}

void FbParams::AddRecent(const wxString &text, const wxString &title)
{
	int i = 0;

	while (i<5) {
		wxString file = GetStr(FB_RECENT_0 + i);
		if (file == text) break;
		i++;
	}

	while (i>0){
		wxString file = GetStr(FB_RECENT_0 + i - 1);
		Set(FB_RECENT_0 + i, file);
		wxString info = GetStr(FB_TITLE_0 + i - 1);
		Set(FB_TITLE_0 + i, info);
		i--;
	}

	Set(FB_RECENT_0 + i, text);
	Set(FB_TITLE_0 + i, title);
}

void FbParams::Reset(int param)
{
	FbCollection::ResetParam(param);
}

int FbParams::Param(wxWindowID winid, int param)
{
	bool ok = (ID_FRAME_AUTH <= winid && winid <= ID_FRAME_FIND && 0 <= param && param < FB_FRAME_OFFSET);
	int delta = winid - ID_FRAME_AUTH + 1;
	return ok ? (param + FB_FRAME_OFFSET * delta) : 0;
}

int FbParams::GetInt(wxWindowID winid, int param)
{
	int id = Param(winid, param);
	return id ? GetInt(id) : 0;
}

wxString FbParams::GetStr(wxWindowID winid, int param)
{
	int id = Param(winid, param);
	return id ? GetStr(id) : wxString();
}

wxString FbParams::GetPath(int param)
{
	wxFileName path = GetStr(param);
	if (path.IsRelative()) path.MakeAbsolute(FbDatabase::GetConfigPath());
	return path.GetFullPath();
}

void FbParams::Set(wxWindowID winid, int param, int value)
{
	int id = Param(winid, param);
	if (id) Set(id, value);
}

void FbParams::Set(wxWindowID winid, int param, const wxString &text)
{
	int id = Param(winid, param);
	if (id) Set(id, text);
}

bool FbParams::IsGenesis()
{
	return FbParams::GetStr(DB_LIBRARY_TYPE) == wxT("GENESIS");
}

