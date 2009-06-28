#include <wx/stdpaths.h>
#include "FbParams.h"
#include "MyRuLibApp.h"

FbParams::FbParams()
    : m_database(wxGetApp().GetDatabase()), m_locker(wxGetApp().m_DbSection)
{};

FbParams::FbParams(DatabaseLayer *database, wxCriticalSection &section)
        :    m_database(database), m_locker(section)
{};

int FbParams::GetValue(const int &param)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    if (row->IsNew())
        return DefaultValue(param);
    else
        return row->value;
};

wxString FbParams::GetText(const int &param)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    if (row->IsNew())
        return DefaultText(param);
    else
        return row->text;
};

void FbParams::SetValue(const int &param, int value)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    if (value == DefaultValue(param)) {
        row->Delete();
    } else {
        row->value = value;
        row->Save();
    }
};

void FbParams::SetText(const int &param, wxString text)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    row->Save();
    if (text.IsEmpty() || text == DefaultText(param)) {
        row->Delete();
    } else {
        row->text = text;
        row->Save();
    }
};

int FbParams::DefaultValue(int param)
{
    switch (param) {
        case FB_TRANSLIT_FOLDER: return 0;
        case FB_TRANSLIT_FILE: return 1;
        case FB_USE_PROXY: return 0;
        case FB_FB2_ONLY: return 1;
    }

    return 0;
};

wxString FbParams::DefaultText(int param)
{
    switch (param) {
        case FB_LIBRARY_DIR: return wxGetApp().GetAppPath();
        case FB_EXTRACT_DIR: return wxStandardPaths().GetTempDir();
        case FB_DOWNLOAD_DIR: return wxGetApp().GetAppPath() + wxT("download");
#ifndef __WIN32__
        case FB_FB2_PROGRAM: return wxT("fbreader");
#endif //__WIN32__
    }

    return wxEmptyString;
};

