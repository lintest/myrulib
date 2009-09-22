#include "BookExtractInfo.h"
#include "MyRuLibApp.h"

WX_DEFINE_OBJARRAY(BookExtractInfoArray);

BookExtractInfo::BookExtractInfo(wxSQLite3ResultSet & result):
	id_book(result.GetInt(wxT("id"))),
	id_archive(result.GetInt(wxT("id_archive"))),
    book_name(result.GetString(wxT("file_name"))),
    book_path(result.GetString(wxT("file_path")))
{
}

wxString BookExtractInfo::GetBook()
{
    wxString result = book_path;
    if (!result.IsEmpty()) result += wxFileName::GetPathSeparator();
    result += book_name;
    return result;
}

wxString BookExtractInfo::GetZip(const wxString &path)
{
    wxString result = path.IsEmpty() ? zip_path : path;
    if (!result.IsEmpty()) result += wxFileName::GetPathSeparator();
    result += zip_name;
    return result;
}

bool BookExtractInfo::ZipInclude()
{
    wxFileName filename = zip_name;
    return filename.GetName() == book_name;
}
