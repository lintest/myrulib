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
		int GetId() const { return m_id; };
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

class FbItemData: public wxTreeItemData
{
	public:
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const = 0;
		virtual void Open() const = 0;
		virtual int GetId() const { return 0; };
};

class FbBookData: public FbItemData
{
	public:
		FbBookData(int id): m_id(id) {};
		FbBookData(const FbBookData & data): m_id(data.m_id), m_filetype(data.m_filetype) {};
		FbBookData(const BookTreeItemData & data): m_id(data.GetId()), m_filetype(data.file_type) {};
		bool operator == (const FbBookData & data) const { return m_id == data.m_id; };
		virtual int GetId() const { return m_id; };
		virtual void Show(wxEvtHandler * frame, bool bVertical, bool bEditable = false) const;
		virtual void Open() const;
	private:
		void DoDownload() const;
	private:
		int m_id;
	public:
		wxString m_filetype;
};

#endif // __FBBOOKDATA_H__
