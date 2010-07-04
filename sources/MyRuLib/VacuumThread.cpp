#include "VacuumThread.h"
#include "FbDatabase.h"

void VacuumThread::Execute()
{
	if ( Create() != wxTHREAD_NO_ERROR ) Run();
}

void * VacuumThread::Entry()
{
	wxLogMessage(_("Start SQLite VACUUM"));
	FbCommonDatabase().ExecuteUpdate(wxT("VACUUM"));
	FbConfigDatabase().ExecuteUpdate(wxT("VACUUM"));
	wxLogMessage(wxT("Finish SQLite VACUUM"));
	return NULL;
}
