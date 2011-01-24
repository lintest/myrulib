#ifndef __FBEXTRACTINFO_H__
#define __FBEXTRACTINFO_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/arrimpl.cpp>
#include <wx/wxsqlite3.h>

class FbExtractItem
{
	public:
		FbExtractItem(wxSQLite3ResultSet & result, int id, const wxString & ext, const wxString & md5);
		FbExtractItem(const FbExtractItem & item);
	public:
		wxString FileName(bool bInfoOnly) const;
		wxString ErrorName() const;
		void DeleteFile(const wxString &basepath) const;
		bool FindZip(const wxString &basepath, wxFileName &filename) const;
		bool FindBook(const wxString &basepath, wxFileName &filename) const;
	public:
		int id_book;
		int id_archive;
		wxString book_name;
		wxString book_path;
		wxString file_type;
		wxString zip_name;
		wxString zip_path;
		bool librusec;
	private:
		wxFileName GetBook(const wxString &path = wxEmptyString) const;
		wxFileName GetZip(const wxString &path = wxEmptyString) const;
		wxString InfoName() const;
		bool NotFb2() const;
};

WX_DECLARE_OBJARRAY(FbExtractItem, FbExtractArrayBase);

class FbExtractArray
	: public FbExtractArrayBase
{
	public:
		FbExtractArray(wxSQLite3Database & database, const int id);
		int GetId() const {return m_id; };
		void DeleteFiles(const wxString &basepath) const;
	private:
		int m_id;
};

#endif // __FBEXTRACTINFO_H__
