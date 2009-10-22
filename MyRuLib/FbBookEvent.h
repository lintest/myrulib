#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include "FbBookData.h"

DECLARE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION, 1 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION, 2 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION, 3 )

enum FbFolderType {
	FT_FOLDER,
	FT_RATING,
	FT_COMMENT,
	FT_DOWNLOAD,
};

class FbCommandEvent: public wxCommandEvent
{
	public:
		FbCommandEvent(wxEventType commandType, int winid, const wxString &sting = wxEmptyString)
			: wxCommandEvent(commandType, winid) { SetString(sting); };
		FbCommandEvent(const wxCommandEvent& event)
			: wxCommandEvent(event) {};
	public:
		void Post(wxEvtHandler *dest);
		void Post();
};

class FbBookEvent: public FbCommandEvent
{
	public:
		FbBookEvent(wxWindowID commandId, BookTreeItemData * data, const wxString &sting = wxEmptyString)
			: FbCommandEvent(fbEVT_BOOK_ACTION, commandId, sting), m_data(data) {};

		FbBookEvent(const FbBookEvent & event)
			: FbCommandEvent(event), m_data(event.m_data) {};

		virtual wxEvent *Clone() const { return new FbBookEvent(*this); }

	public:
		BookTreeItemData m_data;
};

class FbOpenEvent: public FbCommandEvent
{
	public:
		FbOpenEvent(wxWindowID commandId, int author, int book)
			: FbCommandEvent(fbEVT_OPEN_ACTION, commandId), m_author(author), m_book(book) {};

		FbOpenEvent(const FbOpenEvent & event)
			: FbCommandEvent(event), m_author(event.m_author), m_book(event.m_book) {};

		virtual wxEvent *Clone() const { return new FbOpenEvent(*this); }

	public:
		int m_author;
		int m_book;
};

class FbFolderEvent: public FbCommandEvent
{
	public:
		FbFolderEvent(wxWindowID commandId, int folder, FbFolderType type)
			: FbCommandEvent(fbEVT_FOLDER_ACTION, commandId), m_folder(folder), m_type(type) {};

		FbFolderEvent(const FbFolderEvent & event)
			: FbCommandEvent(event), m_folder(event.m_folder), m_type(event.m_type) {};

		virtual wxEvent *Clone() const { return new FbFolderEvent(*this); }

	public:
		int m_folder;
		FbFolderType m_type;
};

typedef void (wxEvtHandler::*FbBookEventFunction)(FbBookEvent&);
typedef void (wxEvtHandler::*FbOpenEventFunction)(FbOpenEvent&);
typedef void (wxEvtHandler::*FbFolderEventFunction)(FbFolderEvent&);

#define EVT_FB_BOOK(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_BOOK_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbBookEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_OPEN(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_OPEN_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbOpenEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_FOLDER(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_FOLDER_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbFolderEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
