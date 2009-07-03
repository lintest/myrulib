#include <wx/stdpaths.h>
#include "FbParams.h"
#include "MyRuLibApp.h"

WX_DEFINE_OBJARRAY(ParamArray);

FbParams::FbParams()
    : m_database(wxGetApp().GetDatabase())
{};

ParamArray & FbParams::GetParams()
{
    static ParamArray params;
    return params;
}

void FbParams::LoadParams()
{
    ParamArray & paramList = GetParams();
    paramList.Empty();

    wxCriticalSectionLocker(wxGetApp().m_DbSection);

    Params params(wxGetApp().GetDatabase());
    ParamsRowSet * rowSet = params.All();
    for (size_t i = 0; i<rowSet->Count(); i++) {
        paramList.Add(new ParamItem(rowSet->Item(i)));
    }
}

int FbParams::GetValue(const int &param)
{
    ParamArray & paramList = GetParams();
    for (size_t i=0; i<paramList.Count(); i++) {
        if (paramList[i].id == param) {
            return paramList[i].value;
        }
    }
    return DefaultValue(param);
};

wxString FbParams::GetText(const int &param)
{
    ParamArray & paramList = GetParams();
    for (size_t i=0; i<paramList.Count(); i++) {
        if (paramList[i].id == param) {
            return paramList[i].text;
        }
    }
    return DefaultText(param);
};

void FbParams::SetValue(const int &param, int value)
{
    wxCriticalSectionLocker locker(wxGetApp().m_DbSection);

    Params params(m_database);
    ParamsRow * row = params.Id(param);
    if (value == DefaultValue(param)) {
        row->Delete();
    } else {
        row->value = value;
        row->Save();
    }

    ParamArray & paramList = GetParams();
    for (size_t i=0; i<paramList.Count(); i++) {
        if (paramList[i].id == param) {
            paramList[i].value = value;
            return;
        }
    }

    ParamItem * item = new ParamItem(param);
    item->value = value;
    paramList.Add(item);
}

void FbParams::SetText(const int &param, wxString text)
{
    wxCriticalSectionLocker locker(wxGetApp().m_DbSection);

    Params params(m_database);
    ParamsRow * row = params.Id(param);
    row->Save();
    if (text.IsEmpty() || text == DefaultText(param)) {
        row->Delete();
    } else {
        row->text = text;
        row->Save();
    }

    ParamArray & paramList = GetParams();
    for (size_t i=0; i<paramList.Count(); i++) {
        if (paramList[i].id == param) {
            paramList[i].text = text;
            return;
        }
    }

    ParamItem * item = new ParamItem(param);
    item->text = text;
    paramList.Add(item);
}

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

