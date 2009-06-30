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
#include <wx/listbox.h>
#include <DatabaseLayer.h>
#include "FbGenres.h"
#include "wx/treelistctrl.h"

class AuthorsRowSet;

class BookTreeItemData: public wxTreeItemData
{
public:
	BookTreeItemData(int id): m_id(id) { };
	int GetId() { return m_id; };
private:
	int m_id;
};

class FbManager{
public:
    static wxString GetAnnotation(int id);
	static void FillBooks(wxTreeListCtrl * treelist, int id_author, bool fb2only);
	static void FillAuthorsChar(wxListBox *listbox, const wxChar & findLetter);
	static void FillAuthorsText(wxListBox *listbox, const wxString & findText);
	static void OpenBook(int id);
	static bool ParseXml(const wxString& filename);
	static bool ParseZip(const wxString& filename);
	static bool RegisterZip(const wxString& filename);
private:
	static void FillAuthors(wxListBox *listbox, DatabaseResultSet* result);
};

#endif //FBMANAGER_H
