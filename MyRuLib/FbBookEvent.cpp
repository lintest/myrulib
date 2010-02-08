#include "FbBookEvent.h"
#include "MyRuLibApp.h"

DEFINE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_PROGRESS_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_MASTER_ACTION )

void FbCommandEvent::Post(wxEvtHandler *dest)
{
	wxPostEvent(dest, *this);
}

void FbCommandEvent::Post()
{
	wxPostEvent(wxGetApp().GetTopWindow(), *this);
}

FbMasterEvent::FbMasterEvent(wxWindowID id, wxSQLite3ResultSet &result):
	FbCommandEvent(fbEVT_MASTER_ACTION, id, result.GetString(wxT("name"))),
	m_master(result.GetInt(wxT("id"))),
	m_parent(0),
	m_number(result.GetInt(wxT("number")))
{
}
