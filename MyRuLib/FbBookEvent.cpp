#include "FbBookEvent.h"
#include "MyRuLibApp.h"

DEFINE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION )
DEFINE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION )

void FbBookEvent::Post(wxEvtHandler *dest)
{
    wxPostEvent(dest, *this);
}

void FbFolderEvent::Post()
{
    wxPostEvent(wxGetApp().GetTopWindow(), *this);
}

