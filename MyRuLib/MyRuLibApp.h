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
	bool OpenDatabase(const wxString &filename);
	const wxString GetAppData();
	const wxString GetAppPath();
private:
	void OpenLog();
	wxFileName GetDatabaseFilename(FbDatabase &database);
	void SetAppData(const wxString &filename);
private:
	wxCriticalSection m_section;
	wxString m_datafile;
};

DECLARE_APP(MyRuLibApp)

#endif // MYRULIBAPP_H
