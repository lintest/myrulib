#include "FbBookData.h"

BookTreeItemData::BookTreeItemData(wxSQLite3ResultSet & res):
	m_id( res.GetInt(wxT("id"))),
	title( res.GetString(wxT("title"))),
	file_type( res.GetString(wxT("file_type"))),
	file_size( res.GetInt(wxT("file_size"))),
	number( res.GetInt(wxT("number"))),
	genres( res.GetString(wxT("genres"))),
	rating(0)
{
	int r = res.GetInt(wxT("rating"));
	if ( r>=0 && r<=5 ) rating = r;
}

