/***************************************************************
 * Name:      FbManager.cpp
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/zipstrm.h>
#include <wx/txtstrm.h>
#include "FbManager.h"
#include "MyRuLibApp.h"
#include "Authors.h"
#include "Books.h"
#include "Params.h"

enum {
	DB_LIBRARY_TITLE = 1,
	DB_NEW_AUTHOR_ID,
	DB_NEW_BOOK_ID,
};

void FbManager::MakeLower(wxString & data){
#ifdef __WIN32__
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharLower(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeLower();
#endif
}

int FbManager::NewAuthorId()
{
	Params * params = wxGetApp().GetParams();

	ParamsRow * row = params->Id(DB_NEW_AUTHOR_ID);
	row->value++;
	row->Save();

	return row->value;
}

int FbManager::NewBookId()
{
	Params * params = wxGetApp().GetParams();

	ParamsRow * row = params->Id(DB_NEW_BOOK_ID);
	row->value++;
	row->Save();

	return row->value;
}

int FbManager::FindAuthor(wxString &full_name) {

	Authors * authors = wxGetApp().GetAuthors();

	wxString search_name = full_name;
	MakeLower(search_name);

	const wxString& whereClause = wxString::Format(_("search_name='%s'"), search_name.c_str());

	AuthorsRow * row = authors->Where(whereClause);

	if (!row) {
		row = authors->New();
		row->id = NewAuthorId();
		row->search_name = search_name;
		row->full_name = full_name;
		row->Save();
	}
	return row->id;
}

bool FbManager::ParseXml(const wxString& filename, wxString& html) 
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return false;

	wxFile file(filename);
	wxFileOffset size = file.Length() / 1024;

    return ParseXml(stream, html, filename, size);
}

bool FbManager::ParseZip(const wxString& filename, wxString& html) 
{

	wxZipEntry* entry;
	wxFFileInputStream in(filename);
	wxZipInputStream zip(in);

	while (entry = zip.GetNextEntry()) {
		zip.OpenEntry(*entry);
		ParseXml(zip, html, entry->GetName(), entry->GetSize());
		delete entry;
	}

    return true;
}

bool FbManager::ParseXml(wxInputStream& stream, wxString& html, const wxString &name, const wxFileOffset size) 
{
    FbDocument xml;
	if (!xml.Load(stream, wxT("UTF-8")))
		return false;

#ifdef FB_DEBUG_PARSING
	xml.GetRoot()->Print(html);
#endif //FB_DEBUG_PARSING

	html += wxT("<br>");

	FbNode * node = xml.GetRoot();
	if (!node) return false;

	node = node->Find(wxT("description"));
	if (!node) return false;

	node = node->Find(wxT("title-info"));
	if (!node) return false;

	wxArrayInt book_authors;
	wxArrayString book_genres;
	wxString book_title;

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        wxString value;
        if ( name == wxT("author") ) {
            value = xml.GetAuthor(node);
			book_authors.Add( FindAuthor(value) );
		} else {
			value = node->m_text;
			if ( name == wxT("genre") ) {
				book_genres.Add(value);
			} else if ( name == wxT("book-title") ) {
				book_title = value;
			}
        }
        html += wxString::Format(wxT("<b>%s:</b>&nbsp;%s<br>"), name.c_str(), value.c_str());
		node = node->m_next;
    }

	Books * books = wxGetApp().GetBooks();
	int new_id = NewBookId();

	size_t iConut = book_authors.Count();
	for (size_t i = 0; i<iConut; i++) {
		BooksRow * row = books->New();
		row->id = new_id;
		row->id_author = book_authors[i];
		row->title = book_title;
		row->file_size = size /1024;
		row->file_name = name;
		row->Save();
	}

    html += wxT("<hr>");

	return true;

}
