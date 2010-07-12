#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbBookData.h"
#include "FbCollection.h"
#include "FbConst.h"

int FbParams::GetValue(int param)
{
	return FbCollection::GetParamInt(param);
};

wxString FbParams::GetText(int param)
{
	return FbCollection::GetParamStr(param);
};

void FbParams::SetValue(int param, int value)
{
	return FbCollection::SetParamInt(param, value);
}

void FbParams::SetText(int param, const wxString &text)
{
	return FbCollection::SetParamStr(param, text);
}

int FbParams::DefaultValue(int param)
{
	if (param < FB_FRAME_OFFSET)
		switch (param) {
			case FB_TEMP_DEL: return 1;
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

wxString FbParams::DefaultText(int param)
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
				return FbStandardPaths().GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT("download");
			case FB_TEMP_DIR:
				return FbStandardPaths().GetUserConfigDir() + wxFileName::GetPathSeparator() + wxT("local");
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
	wxString info = GetText(param);
	if (info.IsEmpty()) return wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font;
	font.SetNativeFontInfo(info);
	return font;
}

void FbParams::AddRecent(const wxString &text, const wxString &title)
{
	int i = 0;

	while (i<5) {
		wxString file = GetText(FB_RECENT_0 + i);
		if (file == text) break;
		i++;
	}

	while (i>0){
		wxString file = GetText(FB_RECENT_0 + i - 1);
		SetText(FB_RECENT_0 + i, file);
		wxString info = GetText(FB_TITLE_0 + i - 1);
		SetText(FB_TITLE_0 + i, info);
		i--;
	}

	SetText(FB_RECENT_0 + i, text);
	SetText(FB_TITLE_0 + i, title);
}

void FbParams::ResetValue(int param)
{
	SetValue(param, DefaultValue(param));
}

int FbParams::Param(wxWindowID winid, int param)
{
	bool ok = (ID_FRAME_AUTHOR <= winid && winid <= ID_FRAME_SEARCH && 0 <= param && param < FB_FRAME_OFFSET);
	int delta = winid - ID_FRAME_AUTHOR + 1;
	return ok ? (param + FB_FRAME_OFFSET * delta) : 0;
}

int FbParams::GetValue(wxWindowID winid, int param)
{
	int id = Param(winid, param);
	return id ? GetValue(id) : 0;
}

wxString FbParams::GetText(wxWindowID winid, int param)
{
	int id = Param(winid, param);
	return id ? GetText(id) : wxString();
}

void FbParams::SetValue(wxWindowID winid, int param, int value)
{
	int id = Param(winid, param);
	if (id) SetValue(id, value);
}

void FbParams::SetText(wxWindowID winid, int param, const wxString &text)
{
	int id = Param(winid, param);
	if (id) SetText(id, text);
}

