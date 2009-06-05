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
#include "FbParser.h"

class FbManager{
public:
    void MakeLower(wxString & data);
	int FindAuthor(wxString &full_name);
	int NewAuthorId();
	int NewBookId();
	bool Parse(const wxString& filename, wxString& html);
};

#endif //FBMANAGER_H
