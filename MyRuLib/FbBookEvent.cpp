#include "FbBookEvent.h"
#include "MyRuLibApp.h"

DEFINE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_MODEL_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_PROGRESS_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_MASTER_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_EXPORT_ACTION )

void FbCommandEvent::Post(wxEvtHandler *dest)
{
	wxPostEvent(dest, *this);
}

void FbCommandEvent::Post()
{
	wxPostEvent(wxGetApp().GetTopWindow(), *this);
}

