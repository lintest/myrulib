#include "VacuumThread.h"
#include "FbDatabase.h"

void VacuumThread::Execute()
{
	if ( Create() != wxTHREAD_NO_ERROR ) Run();
}

void * VacuumThread::Entry()
{
	wxLogInfo(wxT("Start SQLite VACUUM"));
	FbCommonDatabase().ExecuteUpdate(wxT("VACUUM"));
	FbConfigDatabase().ExecuteUpdate(wxT("VACUUM"));
	wxLogInfo(wxT("Finish SQLite VACUUM"));
	return NULL;
}
