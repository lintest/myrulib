/***************************************************************
 * Name:      DBCreator.h
 * Purpose:   Code for Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <wx/wx.h>
#include <DatabaseLayer.h>
#include <SqliteDatabaseLayer.h>

class DBCreator
{
private:
	DatabaseLayer * m_Database;
public:
	DBCreator(DatabaseLayer * database);
	~DBCreator(void);
	bool CreateDatabase(const wxString & filename);
	bool UpgradeDatabase();
};

#endif // DBCREATOR_H
