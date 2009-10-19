#include "FbParams.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "MyRuLibApp.h"
#include "FbParams.h"
#include "ZipReader.h"
#include "ImpThread.h"

WX_DEFINE_OBJARRAY(ParamArray);

ParamItem::ParamItem(wxSQLite3ResultSet & result):
    id(result.GetInt(wxT("id"))),
    value(result.GetInt(wxT("value"))),
    text(result.GetString(wxT("text")))
{
}

ParamArray FbParams::sm_params;

wxCriticalSection FbParams::sm_queue;

FbParams::FbParams()
{
	m_database.AttachConfig();
}

void FbParams::LoadParams()
{
    wxCriticalSectionLocker enter(sm_queue);

    sm_params.Empty();

	wxString sql = wxT("SELECT id, value, text FROM config WHERE id>=100 UNION ALL SELECT id, value, text FROM params WHERE id<100");
	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	while (result.NextRow()) sm_params.Add(new ParamItem(result));

	ZipReader::Init();
}

int FbParams::GetValue(const int param)
{
    wxCriticalSectionLocker enter(sm_queue);
    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            return sm_params[i].value;
        }
    }
    return DefaultValue(param);
};

wxString FbParams::GetText(const int param)
{
    wxCriticalSectionLocker enter(sm_queue);
    for (size_t i=0; i<sm_params.Count(); i++) {
        if (sm_params[i].id == param) {
            return sm_params[i].text;
        }
    }
    return DefaultText(param);
};

void FbParams::SetValue(const int param, int value)
{
    wxCriticalSectionLocker enter(sm_queue);

    const wchar_t * table = param < 100 ? wxT("params") : wxT("config");

    if (value == DefaultValue(param)) {
        wxString sql = wxString::Format( wxT("DELETE FROM %s WHERE id=?"), table);
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, param);
        stmt.ExecuteUpdate();
    } else {
        wxString sql = wxString::Format( wxT("INSERT OR REPLACE INTO %s (value, id) VALUES (?,?)"), table);
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, value);
        stmt.Bind(2, param);
        stmt.ExecuteUpdate();
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

void FbParams::SetText(const int param, wxString text)
{
    wxCriticalSectionLocker enter(sm_queue);

    const wchar_t * table = param < 100 ? wxT("params") : wxT("config");

    if (text == DefaultValue(param)) {
        wxString sql = wxString::Format( wxT("DELETE FROM %s WHERE id=?"), table);
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, param);
        stmt.ExecuteUpdate();
    } else {
        wxString sql = wxString::Format( wxT("INSERT OR REPLACE INTO %s (text, id) VALUES (?,?)"), table);
        wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
        stmt.Bind(1, text);
        stmt.Bind(2, param);
        stmt.ExecuteUpdate();
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
        case FB_MODE_AUTHOR: return 1;
        case FB_TRANSLIT_FOLDER: return 0;
        case FB_TRANSLIT_FILE: return 1;
        case FB_USE_PROXY: return 0;
        default: return 0;
    }
};

wxString FbParams::DefaultText(int param)
{
    switch (param) {
        case DB_LIBRARY_DIR:
            return wxGetApp().GetAppPath();
        case FB_DOWNLOAD_DIR: {
            wxFileName filename;
            filename.SetName(wxT("download"));
            filename.SetPath(wxGetApp().GetAppPath());
            return filename.GetFullPath();
        }
        case DB_WANRAIK_DIR:
            return wxGetApp().GetAppPath();
		case FB_LIBRUSEC_ADDR:
            return wxT("http://lib.rus.ec");
        default:
			return wxEmptyString;
    }
};

