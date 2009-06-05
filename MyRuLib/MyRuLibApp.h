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
#include "Authors.h"
#include "Books.h"
#include "Params.h"

class MyRuLibApp : public wxApp
{
	DatabaseLayer * m_Database;
public:
	virtual bool OnInit();
	virtual int OnExit();

	bool ConnectToDatabase();
	bool CreateDatabase();
	DatabaseLayer * GetDatabase() {return m_Database;};
public:
    wxCriticalSection m_critsect;
};

DECLARE_APP(MyRuLibApp)

#endif // MYRULIBAPP_H
