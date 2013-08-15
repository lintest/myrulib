#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include "wx/FbSQLite3.h"
#include "FbBookTypes.h"
#include "FbMasterInfo.h"
#include "FbMasterTypes.h"

DECLARE_LOCAL_EVENT_TYPE( fbEVT_BOOK_ACTION,     1 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_MODEL_ACTION,    2 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_OPEN_ACTION,     3 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_FOLDER_ACTION,   4 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_PROGRESS_ACTION, 5 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_MASTER_ACTION,   6 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_EXPORT_ACTION,   7 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_ARRAY_ACTION,    8 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_COUNT_ACTION,    9 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_IMAGE_ACTION,   10 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_LETTERS_ACTION, 11 )
DECLARE_LOCAL_EVENT_TYPE( fbEVT_INIT_FRAMES,    12 )

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
			: FbCommandEvent(fbEVT_MASTER_ACTION, id), m_book(0), m_add(true) {};

		FbMasterEvent(const FbMasterEvent & event)
			: FbCommandEvent(event), m_info(event.m_info), m_book(event.m_book), m_add(event.m_add) {};

		FbMasterEvent(wxWindowID id, const FbMasterInfo &info, int book, bool add)
			: FbCommandEvent(fbEVT_MASTER_ACTION, id), m_info(info), m_book(book), m_add(add) {};

		virtual wxEvent *Clone() const { return new FbMasterEvent(*this); }

	public:
		FbMasterInfo m_info;
		int m_book;
		bool m_add;
};

class FbOpenEvent: public FbCommandEvent
{
	public:
		FbOpenEvent(wxWindowID id, int code, int book = 0)
			: FbCommandEvent(fbEVT_OPEN_ACTION, id), m_code(code), m_book(book) {};

		FbOpenEvent(const FbOpenEvent & event)
			: FbCommandEvent(event), m_code(event.m_code), m_book(event.m_book) {};

		virtual wxEvent *Clone() const { return new FbOpenEvent(*this); }

	public:
		int m_code;
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

	private:
		wxArrayInt m_array;
		FbSortedArrayInt m_check;
};

class FbCountEvent: public FbCommandEvent
{
	public:
		FbCountEvent(wxWindowID winid)
			: FbCommandEvent(fbEVT_COUNT_ACTION, winid), m_count(0) {}

		FbCountEvent(const FbCountEvent & event)
			: FbCommandEvent(event), m_info(event.m_info), m_count(event.m_count) {}

		FbCountEvent(wxWindowID winid, const FbMasterInfo &info, int count)
			: FbCommandEvent(fbEVT_COUNT_ACTION, winid), m_info(info), m_count(count) {}

		virtual wxEvent *Clone() const
			{ return new FbCountEvent(*this); }

		const FbMasterInfo & GetInfo() const
			{ return m_info; }

		int GetCount() const
			{ return m_count; }

	private:
		FbMasterInfo m_info;
		int m_count;
};

class FbImageEvent: public FbCommandEvent
{
	public:
		FbImageEvent(wxWindowID winid)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, winid) {}

		FbImageEvent(const FbImageEvent & event)
			: FbCommandEvent(event), m_image(event.m_image) {}

		FbImageEvent(wxWindowID winid, const wxImage & image, int id, const wxString &str = wxEmptyString)
			: FbCommandEvent(fbEVT_IMAGE_ACTION, winid, id, str), m_image(image) {}

		virtual wxEvent *Clone() const
			{ return new FbImageEvent(*this); }

		const wxImage & GetImage() const
			{ return m_image; }

	private:
		wxImage m_image;
};

class FbLettersEvent: public FbCommandEvent
{
	public:
		FbLettersEvent(const FbLettersEvent & event)
			: FbCommandEvent(event), m_letters(event.m_letters), m_position(event.m_position), m_divider(event.m_divider) {}

		FbLettersEvent(wxWindowID winid, const wxArrayString & letters, int position, int divider)
			: FbCommandEvent(fbEVT_LETTERS_ACTION, winid), m_letters(letters), m_position(position), m_divider(divider) {}

		virtual wxEvent *Clone() const
			{ return new FbLettersEvent(*this); }

		const wxArrayString & GetLetters() const
			{ return m_letters; }

		int GetPosition() const
			{ return m_position; }

		int GetDivider() const
			{ return m_divider; }

	private:
		wxArrayString m_letters;
		int m_position;
		int m_divider;
};

typedef void (wxEvtHandler::*FbModelEventFunction)(FbModelEvent&);

typedef void (wxEvtHandler::*FbOpenEventFunction)(FbOpenEvent&);

typedef void (wxEvtHandler::*FbFolderEventFunction)(FbFolderEvent&);

typedef void (wxEvtHandler::*FbProgressEventFunction)(FbProgressEvent&);

typedef void (wxEvtHandler::*FbMasterEventFunction)(FbMasterEvent&);

typedef void (wxEvtHandler::*FbArrayEventFunction)(FbArrayEvent&);

typedef void (wxEvtHandler::*FbCountEventFunction)(FbCountEvent&);

typedef void (wxEvtHandler::*FbImageEventFunction)(FbImageEvent&);

typedef void (wxEvtHandler::*FbLettersEventFunction)(FbLettersEvent&);

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

#define EVT_FB_COUNT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_COUNT_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbCountEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_IMAGE(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_IMAGE_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbImageEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_LETTERS(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_LETTERS_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbLettersEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
