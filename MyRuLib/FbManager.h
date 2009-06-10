/***************************************************************
 * Name:      FbManager.h
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef FBMANAGER_H
#define FBMANAGER_H

#include <wx/wx.h>
#include <DatabaseLayer.h>
#include "FbParser.h"
#include "FbGenres.h"

class FbManager{
public:
    static void MakeUpper(wxString & data);
    static void MakeLower(wxString & data);
	static int NewId(int param);
	static void InitParams(DatabaseLayer * database);
	static wxString BookInfo(int id);
	bool ParseXml(const wxString& filename, wxString& html);
	bool ParseZip(const wxString& filename, wxString& html);
};

#endif //FBMANAGER_H
