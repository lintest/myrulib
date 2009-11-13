#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbBookData.h"

DECLARE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION, 1 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION, 2 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION, 3 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_PROGRESS_ACTION, 4 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_AUTHOR_ACTION, 5 )

enum FbFolderType {
	FT_FOLDER = 1,
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
		FbBookEvent(wxWindowID id, BookTreeItemData * data, const wxString &sting = wxEmptyString)
			: FbCommandEvent(fbEVT_BOOK_ACTION, id, sting), m_data(data) {};

		FbBookEvent(const FbBookEvent & event)
			: FbCommandEvent(event), m_data(event.m_data) {};

		virtual wxEvent *Clone() const { return new FbBookEvent(*this); }

	public:
		BookTreeItemData m_data;
};

class FbAuthorEvent: public FbCommandEvent
{
	public:
		FbAuthorEvent(wxWindowID id, int author, int parent, const wxString &name = wxEmptyString)
			: FbCommandEvent(fbEVT_AUTHOR_ACTION, id, name), m_author(author), m_parent(parent) {};

		FbAuthorEvent(const FbAuthorEvent & event)
			: FbCommandEvent(event), m_author(event.m_author), m_parent(event.m_parent) {};

		FbAuthorEvent(wxWindowID id, wxSQLite3ResultSet &result);

		virtual wxEvent *Clone() const { return new FbAuthorEvent(*this); }

	public:
		BookTreeItemData m_author;
		BookTreeItemData m_parent;
};

class FbOpenEvent: public FbCommandEvent
{
	public:
		FbOpenEvent(wxWindowID id, int author, int book)
			: FbCommandEvent(fbEVT_OPEN_ACTION, id), m_author(author), m_book(book) {};

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
		FbFolderEvent(wxWindowID id, int folder, FbFolderType type)
			: FbCommandEvent(fbEVT_FOLDER_ACTION, id), m_folder(folder), m_type(type) {};

		FbFolderEvent(const FbFolderEvent & event)
			: FbCommandEvent(event), m_folder(event.m_folder), m_type(event.m_type) {};

		virtual wxEvent *Clone() const { return new FbFolderEvent(*this); }

	public:
		int m_folder;
		FbFolderType m_type;
};

class FbProgressEvent: public FbCommandEvent
{
	public:
		FbProgressEvent(wxWindowID id, const wxString &str = wxEmptyString, const int pos = 0, const wxString &text = wxEmptyString)
			: FbCommandEvent(fbEVT_PROGRESS_ACTION, id), m_str(str), m_pos(pos), m_text(text) {};

		FbProgressEvent(const FbProgressEvent & event)
			: FbCommandEvent(event), m_str(event.m_str), m_pos(event.m_pos), m_text(event.m_text) {};

		virtual wxEvent *Clone() const { return new FbProgressEvent(*this); }

	public:
		wxString m_str;
		int m_pos;
		wxString m_text;
};

typedef void (wxEvtHandler::*FbBookEventFunction)(FbBookEvent&);

typedef void (wxEvtHandler::*FbOpenEventFunction)(FbOpenEvent&);

typedef void (wxEvtHandler::*FbFolderEventFunction)(FbFolderEvent&);

typedef void (wxEvtHandler::*FbProgressEventFunction)(FbProgressEvent&);

typedef void (wxEvtHandler::*FbAuthorEventFunction)(FbAuthorEvent&);

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

#define EVT_FB_PROGRESS(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_PROGRESS_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbProgressEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_AUTHOR(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_AUTHOR_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbAuthorEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
