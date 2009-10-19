/////////////////////////////////////////////////////////////////////////////
// Name:        httpbuilderthread.h
// Purpose:     HTTP builderThread lets you work with the wxHTTPBuilder class in
//				gui based application using events.
// Author:      Angelo Mandato
// Created:     2005/08/10
// RCS-ID:      $Id: httpbuilderthread.h,v 1.1 2005/08/11 04:05:14 amandato Exp $
// Copyright:   (c) 2005 Angelo Mandato (http://www.spaceblue.com)
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTTPBUILDERTHREAD_H_
#define _WX_HTTPBUILDERTHREAD_H_

// optimization for GCC
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "httpbuilderthread.h"
#endif

/*
Note: The idea behind this thread is to give a programmer
      a simple method to send and receive data through
      wxHTTPBuilder object without blocking the application.
      Keep in mind, this may not be the best way to implement
      threads in your application.  If anything, use this as 
      a guide for your implementation.
*/


// Include files:
#include <wx/thread.h>
#include <wx/event.h>
#include <wx/httpbuilder.h>

class WXDLLIMPEXP_HTTPENGINE wxHTTPBuilderThread;

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_HTTPENGINE, wxEVT_HTTPBUILDERTHREAD_FINISHED, 7795)
END_DECLARE_EVENT_TYPES()

class WXDLLIMPEXP_HTTPENGINE wxHTTPBuilderEvent : public wxEvent
{
public:

  wxHTTPBuilderEvent(WXTYPE commandEventType = wxEVT_HTTPBUILDERTHREAD_FINISHED, int nId = 0, bool finished = false, const wxString &data = wxEmptyString )
  : wxEvent(nId)
  {
      m_eventType = commandEventType;
      m_finished = finished;
      m_data = data; // Warning, this could be very large...
  }

  bool GetFinished() { return m_finished; };
  wxString  GetData() { return m_data; };

  virtual wxEvent *Clone() const { return new wxHTTPBuilderEvent(*this); }
private:

  bool  m_finished;
  wxString m_data;
};

typedef void (wxEvtHandler::*wxHTTPBuilderEventFunction)(wxHTTPBuilderEvent&);


#define wxHTTPBuilderEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxHTTPBuilderEventFunction, &func)

#define EVT_HTTPBUILDER_FINISHED(id, func) \
   wx__DECLARE_EVT1(wxEVT_HTTPBUILDERTHREAD_FINISHED, id, wxHTTPBuilderEventHandler(func))

class WXDLLIMPEXP_HTTPENGINE wxHTTPBuilderThreadObj : public wxHTTPBuilder
{
public:
  void SetThread( wxHTTPBuilderThread *thread) { m_thread = thread; };
private:

  bool StopCheck(void); // { return m_Stop; };
  wxHTTPBuilderThread *m_thread;
};

class WXDLLIMPEXP_HTTPENGINE wxHTTPBuilderThread : public wxThread
{
public:

  wxHTTPBuilderThread(wxWindow *parent, int id, wxHTTPBuilder *http, const wxString &url);
  ~wxHTTPBuilderThread();

  // Required thread functions:
  virtual void OnExit(void);
	virtual void* Entry(void);

  wxWindow* GetParent(void) { return m_parent; };
  int GetId(void) { return m_id; };

  // Thread properties, call before starting thread
  void SaveToFile( const bool &saveToFile = false, const wxString &filename = wxEmptyString ) { m_saveToFile = saveToFile; m_filename = filename; };
  void SetTempDirOrPrefix(const wxString &tempDir) { m_tempDir = tempDir; };

private:

  wxHTTPBuilderThreadObj  *m_http;
  wxWindow           	*m_parent;
  int                    	m_id;
  wxString             	m_data;
  bool                   	m_saveToFile;
  wxString              	m_url;
  wxString             	m_tempDir;
  wxString              	m_filename;
};

#endif
