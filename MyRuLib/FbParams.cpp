#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "FbParams.h"
#include "MyRuLibApp.h"

WX_DEFINE_OBJARRAY(ParamArray);

ParamItem::ParamItem(wxSQLite3ResultSet & result):
    id(result.GetInt(wxT("id"))),
    value(result.GetInt(wxT("value"))),
    text(result.GetString(wxT("text")))
{
}

ParamArray FbParams::sm_params;

void FbParams::LoadParams()
{
    sm_params.Empty();

	wxString sql = wxT("SELECT * FROM params");
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    wxSQLite3ResultSet result = wxGetApp().GetDatabase().ExecuteQuery(sql);

    while (result.NextRow()) {
        sm_params.Add(new ParamItem(result));
    }
}

int FbParams::GetValue(const int &param)
{
    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            return sm_params[i].value;
        }
    }
    return DefaultValue(param);
};

wxString FbParams::GetText(const int &param)
{
    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            return sm_params[i].text;
        }
    }
    return DefaultText(param);
};

void FbParams::SetValue(const int &param, int value)
{
    wxCriticalSectionLocker locker(wxGetApp().m_DbSection);

    wxSQLite3Database & database = wxGetApp().GetDatabase();

    if (value == DefaultValue(param)) {
        wxString sql = wxT("DELETE FROM params WHERE id=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, param);
        stmt.ExecuteUpdate();
    } else {
        wxString sql = wxT("UPDATE params SET value=? WHERE id=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, value);
        stmt.Bind(2, param);
        if (!stmt.ExecuteUpdate()) {
            wxString sql = wxT("INSERT INTO params (value, id) VALUES (?,?)");
            wxSQLite3Statement stmt = database.PrepareStatement(sql);
            stmt.Bind(1, value);
            stmt.Bind(2, param);
        }
    }

    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            sm_params[i].value = value;
            return;
        }
    }

    ParamItem * item = new ParamItem(param);
    item->value = value;
    sm_params.Add(item);
}

void FbParams::SetText(const int &param, wxString text)
{
    wxCriticalSectionLocker locker(wxGetApp().m_DbSection);

    wxSQLite3Database & database = wxGetApp().GetDatabase();

    if (text == DefaultValue(param)) {
        wxString sql = wxT("DELETE FROM params WHERE id=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, param);
        stmt.ExecuteUpdate();
    } else {
        wxString sql = wxT("UPDATE params SET text=? WHERE id=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, text);
        stmt.Bind(2, param);
        if (!stmt.ExecuteUpdate()) {
            wxString sql = wxT("INSERT INTO params (text, id) VALUES (?,?)");
            wxSQLite3Statement stmt = database.PrepareStatement(sql);
            stmt.Bind(1, text);
            stmt.Bind(2, param);
        }
    }

    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            sm_params[i].text = text;
            return;
        }
    }

    ParamItem * item = new ParamItem(param);
    item->text = text;
    sm_params.Add(item);
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
        case FB_LIBRARY_DIR:
            return wxGetApp().GetAppPath();
        case FB_EXTRACT_DIR:
            return wxStandardPaths().GetTempDir();
        case FB_DOWNLOAD_DIR: {
            wxFileName filename;
            filename.SetName(wxT("download"));
            filename.SetPath(wxGetApp().GetAppPath());
            return filename.GetFullPath();
        }
        case FB_WANRAIK_DIR:
            return wxGetApp().GetAppPath();
    }
    return wxEmptyString;
};

