#include "FbBookEvent.h"
#include "TestApp.h"

DEFINE_LOCAL_EVENT_TYPE( fbEVT_MODEL_ACTION )

DEFINE_LOCAL_EVENT_TYPE( fbEVT_ARRAY_ACTION )

void FbCommandEvent::Post(wxEvtHandler *dest)
{
	wxPostEvent(dest, *this);
}

void FbCommandEvent::Post()
{
	wxPostEvent(wxGetApp().GetTopWindow(), *this);
}

