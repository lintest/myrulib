#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>
#include "FbBookData.h"
#include "FbBookTypes.h"

DECLARE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION, 1 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_MODEL_ACTION, 2 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION, 3 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION, 4 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_PROGRESS_ACTION, 5 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_MASTER_ACTION, 6 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_EXPORT_ACTION, 7 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_ARRAY_ACTION,  8 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_IMAGE_ACTION,  10 )

class FbModel;

class FbMasterData;

class FbCommandEvent: public wxCommandEvent
{
	public:
		FbCommandEvent(wxEventType commandType, int winid, const wxString &sting = wxEmptyString)
			: wxCommandEvent(commandType, winid) { SetString(sting); };
		FbCommandEvent(wxEventType commandType, int winid, int id, const wxString &sting = wxEmptyString)
			: wxCommandEvent(commandType, winid) { SetInt(id); SetString(sting); };
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

class FbModelEvent: public FbCommandEvent
{
	public:
		FbModelEvent(wxWindowID winid)
			: FbCommandEvent(fbEVT_MODEL_ACTION, winid), m_model(NULL) {}

		FbModelEvent(const FbModelEvent & event)
			: FbCommandEvent(event), m_model(event.m_model) {}

		FbModelEvent(wxWindowID winid, FbModel * model, int index = 0)
			: FbCommandEvent(fbEVT_MODEL_ACTION, winid, index), m_model(model) {}

		virtual wxEvent *Clone() const
			{ return new FbModelEvent(*this); }

		FbModel * GetModel() const
			{ return m_model; }

	public:
		FbModel * m_model;
};

class FbMasterEvent: public FbCommandEvent
{
	public:
		FbMasterEvent(wxWindowID id)
			: FbCommandEvent(fbEVT_MASTER_ACTION, id), m_data(NULL), m_number(0) {};

		FbMasterEvent(const FbMasterEvent & event)
			: FbCommandEvent(event), m_data(event.m_data), m_number(event.m_number) {};

		FbMasterEvent(wxWindowID id, const wxString &text, FbMasterData * data, int number = 0)
			: FbCommandEvent(fbEVT_MASTER_ACTION, id, text), m_data(data), m_number(number) {};

		virtual wxEvent *Clone() const { return new FbMasterEvent(*this); }

	public:
		FbMasterData * m_data;
		int m_number;
};

class FbOpenEvent: public FbCommandEvent
{
	public:
		FbOpenEvent(wxWindowID id, int author, int book = 0)
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

class FbArrayEvent: public FbCommandEvent
{
	public:
		static int CompareInt(int x, int y) { return x - y; }

		FbArrayEvent(wxWindowID winid)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, winid), m_check(CompareInt) {}

		FbArrayEvent(const FbArrayEvent & event)
			: FbCommandEvent(event), m_array(event.m_array), m_check(event.m_check) {}

		FbArrayEvent(wxWindowID winid, const wxArrayInt & array, int index = 0)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, winid, index), m_array(array), m_check(CompareInt) {}

		FbArrayEvent(wxWindowID winid, const wxArrayInt & array, const FbSortedArrayInt &check, int index = 0)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, winid, index), m_array(array), m_check(check) {}

		virtual wxEvent *Clone() const
			{ return new FbArrayEvent(*this); }

		const wxArrayInt & GetArray() const
			{ return m_array; }

	public:
		wxArrayInt m_array;
		FbSortedArrayInt m_check;
};

class FbImageEvent: public FbCommandEvent
{
	public:
		FbImageEvent(wxWindowID winid)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, winid) {}

		FbImageEvent(const FbImageEvent & event)
			: FbCommandEvent(event), m_image(event.m_image) {}

		FbImageEvent(wxWindowID winid, const wxImage & image, const wxString &str = wxEmptyString)
			: FbCommandEvent(fbEVT_IMAGE_ACTION, winid, str), m_image(image) {}

		virtual wxEvent *Clone() const
			{ return new FbImageEvent(*this); }

		const wxImage & GetImage() const
			{ return m_image; }

	public:
		wxImage m_image;
};

typedef void (wxEvtHandler::*FbBookEventFunction)(FbBookEvent&);

typedef void (wxEvtHandler::*FbModelEventFunction)(FbModelEvent&);

typedef void (wxEvtHandler::*FbOpenEventFunction)(FbOpenEvent&);

typedef void (wxEvtHandler::*FbFolderEventFunction)(FbFolderEvent&);

typedef void (wxEvtHandler::*FbProgressEventFunction)(FbProgressEvent&);

typedef void (wxEvtHandler::*FbMasterEventFunction)(FbMasterEvent&);

typedef void (wxEvtHandler::*FbArrayEventFunction)(FbArrayEvent&);

typedef void (wxEvtHandler::*FbImageEventFunction)(FbImageEvent&);

#define EVT_FB_BOOK(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_BOOK_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbBookEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_MODEL(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_MODEL_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbModelEventFunction, & fn ), (wxObject *) NULL ),

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

#define EVT_FB_MASTER(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_MASTER_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbMasterEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_ARRAY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_ARRAY_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbArrayEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_IMAGE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_IMAGE_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbImageEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
