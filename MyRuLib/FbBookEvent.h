#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include "FbBookData.h"

DECLARE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION, 1 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION, 2 )

enum FbFolderType {
	FT_FOLDER,
	FT_RATING,
	FT_COMMENT,
	FT_DOWNLOAD,
};

class FbBookEvent: public wxCommandEvent
{
	public:
		FbBookEvent(wxWindowID commandId, BookTreeItemData * data)
			: wxCommandEvent(fbEVT_BOOK_ACTION, commandId), m_data(data) {};

		FbBookEvent(const FbBookEvent & event)
			: wxCommandEvent(event), m_data(event.m_data) {};

		virtual wxEvent *Clone() const { return new FbBookEvent(*this); }

		void Post(wxEvtHandler *dest);

	public:
		BookTreeItemData m_data;
};

class FbFolderEvent: public wxCommandEvent
{
	public:
		FbFolderEvent(wxWindowID commandId, int folder, FbFolderType type)
			: wxCommandEvent(fbEVT_FOLDER_ACTION, commandId), m_folder(folder), m_type(type) {};

		FbFolderEvent(const FbFolderEvent & event)
			: wxCommandEvent(event), m_folder(event.m_folder), m_type(event.m_type) {};

		virtual wxEvent *Clone() const { return new FbFolderEvent(*this); }

		void Post();

	public:
		int m_folder;
		FbFolderType m_type;
};

typedef void (wxEvtHandler::*FbBookEventFunction)(FbBookEvent&);
typedef void (wxEvtHandler::*FbFolderEventFunction)(FbFolderEvent&);

#define EVT_FB_BOOK(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( fbEVT_BOOK_ACTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( FbBookEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_FOLDER(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( fbEVT_FOLDER_ACTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( FbFolderEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
