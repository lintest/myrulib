/***************************************************************
 * Name:	  FbManager.h
 * Purpose:   Defines Application Frame
 * Author:	Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef FBMANAGER_H
#define FBMANAGER_H

#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/wxsqlite3.h>
#include "wx/treelistctrl.h"

class FbManager
{
	public:
		static void OpenBook(int id, wxString &file_type);
};

class BookInfo
{
public:
	static wxString MakeLower(wxString & data);
	static wxString MakeUpper(wxString & data);
};

#endif //FBMANAGER_H
