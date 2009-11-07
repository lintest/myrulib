#ifndef __BOOKEXTRACTINFO_H__
#define __BOOKEXTRACTINFO_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>
#include "FbDatabase.h"

class BookExtractInfo
{
	public:
		BookExtractInfo(wxSQLite3ResultSet & result);
	public:
		wxFileName GetBook(const wxString &path = wxEmptyString);
		wxFileName GetZip(const wxString &path = wxEmptyString);
		bool NameIsEqual();
	public:
		int id_book;
		int id_archive;
		wxString book_name;
		wxString zip_name;
		bool librusec;
};

WX_DECLARE_OBJARRAY(BookExtractInfo, BookExtractArrayBase);

class BookExtractArray
	: public BookExtractArrayBase
{
	public:
		BookExtractArray(FbDatabase & database, const int id);
		int GetId() {return m_id; };
	private:
		int m_id;
};

#endif // __BOOKEXTRACTINFO_H__
