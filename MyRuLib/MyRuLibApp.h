/***************************************************************
 * Name:      MyRuLibApp.h
 * Purpose:   Defines Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef MYRULIBAPP_H
#define MYRULIBAPP_H

#include <wx/wx.h>
#include <wx/thread.h>
#include "FbDatabase.h"

class MyRuLibApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
    wxString GetAppPath() const;
    wxString GetAppData() const { return m_datafile; };
    wxSQLite3Database & GetDatabase() { return m_database; };
public:
    wxCriticalSection m_DbSection;
private:
	bool ConnectToDatabase();
	bool CreateDatabase();
private:
	wxString m_datafile;
    FbDatabase m_database;
};

DECLARE_APP(MyRuLibApp)

#endif // MYRULIBAPP_H
