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
#include "db/Books.h"

class AuthorsRowSet;

class BookTreeItemData: public wxTreeItemData
{
public:
	BookTreeItemData(BooksRow * row, const wxString &seq = wxEmptyString, int num=0)
        : m_id(row->id), title(row->title), file_size(row->file_size), file_type(row->file_type), sequence(seq), number(num) { };
	BookTreeItemData(BookTreeItemData * data)
        : m_id(data->GetId()), title(data->title), file_size(data->file_size), file_type(data->file_type), sequence(data->sequence), number(data->number) { };
	int GetId() { return m_id; };
private:
	int m_id;
public:
	wxString title;
	int file_size;
	wxString file_type;
	wxString sequence;
	int number;
};

class FbManager
{
	public:
		static wxString GetAnnotation(int id);
		static void FillBooks(wxTreeListCtrl * treelist, int id_author, bool fb2only);
		static void FillAuthorsChar(wxListBox *listbox, const wxChar & findLetter);
		static void FillAuthorsText(wxListBox *listbox, const wxString & findText);
		static void OpenBook(int id, wxString &file_type);
		static wxString GetSystemCommand(const wxString & file_type);
		static wxString GetOpenCommand(const wxString & file_type);
	private:
		static void FillAuthors(wxListBox *listbox, DatabaseResultSet* result);
};

class BookInfo
{
public:
	static int NewId(int param);
    static void MakeLower(wxString & data);
    static void MakeUpper(wxString & data);
};

#endif //FBMANAGER_H
