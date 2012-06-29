#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbCollection.h"
#include "FbConst.h"
#include "controls/FbTreeView.h"

FbParamList FbParams;

int FbParamItem::Int() const
{
	return FbCollection::GetParamInt(m_param);
}

wxString FbParamItem::Str() const
{
	return FbCollection::GetParamStr(m_param);
}

FbParamItem::operator int () const
{
	return Int();
}

FbParamItem::operator wxFont () const
{
	wxString info = Str();
	if (info.IsEmpty()) return wxSystemSettingsNative::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font;
	font.SetNativeFontInfo(info);
	return font;
}

FbParamItem::operator wxString () const
{
	return Str();
}

FbParamItem & FbParamItem::operator = (int value)
{
	FbCollection::SetParamInt(m_param, value);
	return * this;
}

FbParamItem & FbParamItem::operator = (bool value)
{
	FbCollection::SetParamInt(m_param, value ? 1 : 0);
	return * this;
}

FbParamItem & FbParamItem::operator = (const wxString & value)
{
	FbCollection::SetParamStr(m_param, value);
	return * this;
}

int FbParamItem::GetInt(int param)
{
	return FbCollection::GetParamInt(param);
};

wxString FbParamItem::GetStr(int param)
{
	return FbCollection::GetParamStr(param);
};

void FbParamItem::Set(int param, int value)
{
	return FbCollection::SetParamInt(param, value);
}

void FbParamItem::Set(int param, const wxString &text)
{
	return FbCollection::SetParamStr(param, text);
}

int FbParamItem::DefaultInt(int param)
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
			case FB_FILE_LENGTH: return 64;
			case FB_LANG_LOCALE: return wxLANGUAGE_DEFAULT;
			case FB_NUMBER_FORMAT: return 3;
			case FB_STATUS_SHOW: return 0;
			case FB_GRID_HRULES: return 0;
			case FB_GRID_VRULES: return 1;
			case FB_READER_FONT_COLOUR : return 0x000000;
			case FB_READER_BACK_COLOUR : return 0xFFFFE0;
			case FB_READER_FONT_SIZE   : return 20;
			case FB_HEADER_FONT_SIZE   : return 14;
			case FB_READER_SHOW_HEADER : return 1;
			case FB_READER_INTERLINE   : return 100;
			case FB_READER_PAGE_COUNT  : return 1;
			case FB_USE_COOLREADER     : return 1;
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

wxString FbParamItem::DefaultStr(int param)
{
	if (param < FB_FRAME_OFFSET)
		switch (param) {
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
				return wxT("CEF");
			case (FB_FRAME_OFFSET * 3 + FB_BOOK_COLUMNS):
				return wxT("AEF");
			case (FB_FRAME_OFFSET * 5 + FB_BOOK_COLUMNS):
				return wxT("ALEF");
		}
		switch (param % FB_FRAME_OFFSET) {
			case FB_BOOK_COLUMNS:
				return wxT("ACEF");
			default:
				return wxEmptyString;
		}
	}
};

wxColour FbParamItem::GetColour(int param)
{
	return wxColour(GetStr(param));
}

void FbParamItem::AddRecent(const wxString &text, const wxString &title)
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

void FbParamItem::Reset(int param)
{
	FbCollection::ResetParam(param);
}

int FbParamItem::Param(wxWindowID winid, int param)
{
	bool ok = (ID_FRAME_AUTH <= winid && winid <= ID_FRAME_FIND && 0 <= param && param < FB_FRAME_OFFSET);
	int delta = winid - ID_FRAME_AUTH + 1;
	return ok ? (param + FB_FRAME_OFFSET * delta) : 0;
}

wxString FbParamItem::GetPath(int param)
{
	wxFileName path = GetStr(param);
	if (path.IsRelative()) path.MakeAbsolute(FbDatabase::GetConfigPath());
	return path.GetFullPath();
}

void FbParamItem::Set(wxWindowID winid, int param, int value)
{
	int id = Param(winid, param);
	if (id) Set(id, value);
}

void FbParamItem::Set(wxWindowID winid, int param, const wxString &text)
{
	int id = Param(winid, param);
	if (id) Set(id, text);
}

bool FbParamItem::IsGenesis()
{
	return FbParamItem(DB_LIBRARY_TYPE).Str() == wxT("GENESIS");
}

long FbParamList::Style(long style) const
{
	return style
		| (FbParams(FB_GRID_VRULES) ? fbTR_VRULES : 0)
		| (FbParams(FB_GRID_HRULES) ? fbTR_HRULES : 0)
	;
}
