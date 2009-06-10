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
#include "FbParser.h"
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
    static void MakeUpper(wxString & data);
    static void MakeLower(wxString & data);
	static void InitParams(DatabaseLayer * database);
	static wxString BookInfo(int id);
	static void FillBooks(wxTreeListCtrl * treelist, int id_author);
	static void FillAuthors(wxListBox *listbox, const wxString & findText);
	static void FillAuthors(wxListBox *listbox, const wxChar & findLetter);
	static void OpenBook(int id);
	bool ParseXml(const wxString& filename, wxString& html);
	bool ParseZip(const wxString& filename, wxString& html);
private:
	static void FillAuthors(wxListBox *listbox, AuthorsRowSet * allAuthors);
};

#endif //FBMANAGER_H
