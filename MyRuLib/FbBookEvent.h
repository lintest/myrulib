#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include "FbBookData.h"

class FbBookEvent: public wxCommandEvent
{
	public:
		FbBookEvent(wxEventType commandType, wxWindowID commandId, BookTreeItemData * data)
			: wxCommandEvent(commandType, commandId), m_data(data) {};

		FbBookEvent(const FbBookEvent & event)
			: wxCommandEvent(event), m_data(event.m_data) {};

		virtual wxEvent *Clone() const { return new FbBookEvent(*this); }

		BookTreeItemData m_data;
};

DECLARE_EVENT_TYPE( fbEVT_BOOK_ACTION, -1 )

typedef void (wxEvtHandler::*FbBookEventFunction)(FbBookEvent&);

#define EVT_FB_BOOK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( fbEVT_BOOK_ACTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( FbBookEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
