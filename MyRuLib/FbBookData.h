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
		number(data->number)
	{ };
	BookTreeItemData(wxSQLite3ResultSet & result);
	BookTreeItemData(int id): m_id(id) {};
	int GetId() { return m_id; };
private:
	int m_id;
public:
	wxString title;
	wxString file_type;
	int file_size;
	wxString sequence;
	int number;
};

#endif // __FBBOOKDATA_H__
