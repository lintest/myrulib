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
#include "MyrulibData.h"
#include "db/Authors.h"
#include "db/Books.h"
#include "db/Params.h"

class MyRuLibApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
    wxString GetAppPath() const;
    wxString GetAppData() const { return m_datafile; };
	SqliteDatabaseLayer * GetDatabase() {return &m_database; };
public:
    wxCriticalSection m_DbSection;
private:
	bool ConnectToDatabase();
	bool CreateDatabase();
private:
	MyrulibDatabaseLayer m_database;
	wxString m_datafile;
};

DECLARE_APP(MyRuLibApp)

class AutoTransaction {
    public:
        AutoTransaction() { try { wxGetApp().GetDatabase()->BeginTransaction(); } catch (...) {}; };
        ~AutoTransaction() { try { wxGetApp().GetDatabase()->Commit(); } catch (...) {}; };
};

#endif // MYRULIBAPP_H
