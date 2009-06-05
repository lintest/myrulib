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

extern wxString alphabet;

class FbManager{
public:
    static void MakeUpper(wxString & data);
    static void MakeLower(wxString & data);
	static int FindAuthor(wxString &full_name);
	static int NewAuthorId();
	static int NewBookId();
	static void InitParams(DatabaseLayer * database);
	bool ParseXml(wxInputStream& stream, wxString& html, const wxString &name, const wxFileOffset size);
	bool ParseXml(const wxString& filename, wxString& html);
	bool ParseZip(const wxString& filename, wxString& html);
};

#endif //FBMANAGER_H
