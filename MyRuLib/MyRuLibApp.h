/***************************************************************
 * Name:	  MyRuLibApp.h
 * Purpose:   Defines Application Class
 * Author:	Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef MYRULIBAPP_H
#define MYRULIBAPP_H

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include "FbDatabase.h"

class MyRuLibApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
	wxString GetAppData() const { return m_datafile; };
	wxString GetAppPath() const { return wxFileName(m_datafile).GetPath(); };
	bool OpenConfig();
	bool OpenDatabase(const wxString &filename, bool bCreateNew);
public:
	wxCriticalSection m_DbSection;
private:
	bool ConnectToDatabase();
	bool CreateDatabase();
private:
	wxString m_datafile;
};

DECLARE_APP(MyRuLibApp)

#endif // MYRULIBAPP_H
