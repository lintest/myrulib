#include "FbParams.h"
#include "MyRuLibApp.h"

FbParams::FbParams()
    : m_database(wxGetApp().GetDatabase()), m_locker(wxGetApp().m_DbSection)
{};

FbParams::FbParams(DatabaseLayer *database, wxCriticalSection &section)
        :    m_database(database), m_locker(section)
{};

int FbParams::GetValue(int param)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    row->value++;
    row->Save();

    return row->value;
};

wxString FbParams::GetText(int param)
{
    Params params(m_database);
    ParamsRow * row = params.Id(param);
    return row->text;
};
