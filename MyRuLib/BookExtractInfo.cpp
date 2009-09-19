#include "BookExtractInfo.h"
#include "MyRuLibApp.h"

WX_DEFINE_OBJARRAY(BookExtractInfoArray);

BookExtractInfo::BookExtractInfo(DatabaseResultSet* result):
	id_book(result->GetResultInt(wxT("id"))),
	id_archive(result->GetResultInt(wxT("id_archive"))),
    book_name(result->GetResultString(wxT("file_name"))),
    book_path(result->GetResultString(wxT("file_path")))
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

