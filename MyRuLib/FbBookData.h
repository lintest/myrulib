#ifndef __FBBOOKDATA_H__
#define __FBBOOKDATA_H__

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/wxsqlite3.h>

class BookTreeItemData: public wxTreeItemData
{
	public:
		BookTreeItemData(BookTreeItemData * data):
			m_id(data->GetId()),
			title(data->title),
			file_type(data->file_type),
			file_size(data->file_size),
			sequence(data->sequence),
			number(data->number),
			genres(data->genres),
			rating(data->rating),
			language(data->language)
		{ };
		BookTreeItemData(wxSQLite3ResultSet & result);
		int GetId() { return m_id; };
	private:
		int m_id;
	public:
		wxString title;
		wxString file_type;
		int file_size;
		wxString sequence;
		int number;
		wxString genres;
		int rating;
		wxString language;
};

class FbBookData: public wxTreeItemData
{
	public:
		FbBookData(int book, int author = 0): m_book(book), m_author(author) {};
		FbBookData(BookTreeItemData & data): m_book(data.GetId()), m_author(0), m_filetype(data.file_type) {};
		int GetId() { return m_book; };
		int GetAuthor() { return m_author; };
	private:
		int m_book;
		int m_author;
	public:
		wxString m_filetype;
};

#endif // __FBBOOKDATA_H__
