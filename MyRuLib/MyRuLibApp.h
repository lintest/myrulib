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
#include <DatabaseLayer.h>
#include <SqliteDatabaseLayer.h>
#include "db/Authors.h"
#include "db/Books.h"
#include "db/Params.h"

class MyRuLibApp : public wxApp
{
private:
	DatabaseLayer * m_Database;
public:
	virtual bool OnInit();
	virtual int OnExit();
	bool ConnectToDatabase();
	bool CreateDatabase();
    wxString GetAppPath();
	DatabaseLayer * GetDatabase() {return m_Database;};
public:
    wxCriticalSection m_DbSection;
};

DECLARE_APP(MyRuLibApp)

class AutoTransaction {
    public:
        AutoTransaction()  { wxGetApp().GetDatabase()->BeginTransaction(); };
        ~AutoTransaction() { wxGetApp().GetDatabase()->Commit(); };
};

#endif // MYRULIBAPP_H
