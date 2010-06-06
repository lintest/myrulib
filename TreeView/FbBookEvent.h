#ifndef __FBBOOKEVENT_H__
#define __FBBOOKEVENT_H__

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

DECLARE_LOCAL_EVENT_TYPE( fbEVT_MODEL_ACTION, 1 )

DECLARE_LOCAL_EVENT_TYPE( fbEVT_ARRAY_ACTION, 2 )

class FbModel;

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
		FbModelEvent(wxWindowID id)
			: FbCommandEvent(fbEVT_MODEL_ACTION, id), m_model(NULL) {}

		FbModelEvent(const FbModelEvent & event)
			: FbCommandEvent(event), m_model(event.m_model) {}

		FbModelEvent(wxWindowID id, FbModel * model, const wxString &text = wxEmptyString)
			: FbCommandEvent(fbEVT_MODEL_ACTION, id, text), m_model(model) {}

		virtual wxEvent *Clone() const
			{ return new FbModelEvent(*this); }

		FbModel * GetModel() const
			{ return m_model; }

	public:
		FbModel * m_model;
};

class FbArrayEvent: public FbCommandEvent
{
	public:
		FbArrayEvent(wxWindowID id)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, id) {}

		FbArrayEvent(const FbArrayEvent & event)
			: FbCommandEvent(event), m_array(event.m_array) {}

		FbArrayEvent(wxWindowID id, wxArrayInt & array, const wxString &text = wxEmptyString)
			: FbCommandEvent(fbEVT_ARRAY_ACTION, id, text), m_array(array) {}

		virtual wxEvent *Clone() const
			{ return new FbArrayEvent(*this); }

		const wxArrayInt & GetArray() const
			{ return m_array; }

	public:
		wxArrayInt m_array;
};

typedef void (wxEvtHandler::*FbModelEventFunction)(FbModelEvent&);

typedef void (wxEvtHandler::*FbArrayEventFunction)(FbArrayEvent&);

#define EVT_FB_MODEL(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_MODEL_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbModelEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_FB_ARRAY(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( fbEVT_ARRAY_ACTION, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( FbArrayEventFunction, & fn ), (wxObject *) NULL ),

#endif // __FBBOOKEVENT_H__
