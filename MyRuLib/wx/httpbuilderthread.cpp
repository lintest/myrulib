/////////////////////////////////////////////////////////////////////////////
// Name:        httpbuilderthread.cpp
// Purpose:     HTTP builderThread lets you work with the wxHTTPBuilder class in
//				gui based application using events.
// Author:      Angelo Mandato
// Created:     2005/08/11
// RCS-ID:      $Id: httpbuilderthread.cpp,v 1.1 2005/08/12 04:12:49 amandato Exp $
// Copyright:   (c) 2002-2005 Angelo Mandato
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "httpbuilderthread.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
	// here goes the #include <wx/abc.h> directives for those
	// files which are not included by wxprec.h
	#include <wx/wx.h>
	#include <wx/window.h>
#endif

#include <wx/httpbuilderthread.h>

DEFINE_EVENT_TYPE(wxEVT_HTTPBUILDERTHREAD_FINISHED)

//! Check to see if the thread is marked to be stopped
bool wxHTTPBuilderThreadObj::StopCheck(void)
{
  if( m_thread->TestDestroy() )
    m_Stop = true;

  return m_Stop;
}

//! wxHTTPBuilderThread constructor
wxHTTPBuilderThread::wxHTTPBuilderThread(wxWindow *parent, int id, wxHTTPBuilder *http, const wxString &url)
{
  m_parent = parent;
  m_id = id;
  m_url = url;
  m_http = (wxHTTPBuilderThreadObj*)http;
  m_data = wxT("");
  m_saveToFile = false;
}

//! wxHTTPBuilderThread destructor
wxHTTPBuilderThread::~wxHTTPBuilderThread()
{
  // Empty
}

//! Thread's OnExit
void wxHTTPBuilderThread::OnExit(void)
{ 
  wxHTTPBuilderEvent event( wxEVT_HTTPBUILDERTHREAD_FINISHED, GetId(), true, m_data );
  event.SetEventObject( m_http );
  m_parent->GetEventHandler()->ProcessEvent(event);
}

//! Thread's Entry
void* wxHTTPBuilderThread::Entry(void)
{
  if( m_saveToFile )
    m_http->SaveFile(m_filename, m_url, m_tempDir);
  else
    m_data = m_http->GetInputString( m_url, m_tempDir);

  return NULL;
}

// eof
