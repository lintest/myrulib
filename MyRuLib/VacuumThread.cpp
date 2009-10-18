#include "VacuumThread.h"
#include "FbDatabase.h"

void VacuumThread::Execute()
{
    wxThread * thread = new VacuumThread();
    if ( thread->Create() != wxTHREAD_NO_ERROR ) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    thread->Run();
}

void * VacuumThread::Entry()
{
    wxLogInfo(wxT("Start SQLite VACUUM"));
    FbCommonDatabase().ExecuteUpdate(wxT("VACUUM"));
    wxLogInfo(wxT("Finish SQLite VACUUM"));
    return NULL;
}
