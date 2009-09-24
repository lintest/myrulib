#include "FbAuthorList.h"
#include "FbClientData.h"
#include "ImpContext.h"
#include "FbManager.h"
#include "MyRuLibApp.h"

void FbAuthorList::FillAuthorsChar(const wxChar & findLetter)
{
	wxString sql = wxT("SELECT id, first_name, middle_name, last_name FROM authors WHERE letter=? ORDER BY search_name");

    FbCommonDatabase database;
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, (wxString)findLetter);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FillAuthors(result);
}

void FbAuthorList::FillAuthorsText(const wxString & findText)
{
	wxString sql = wxT("SELECT id, first_name, middle_name, last_name FROM authors WHERE search_name like ? ORDER BY search_name");
	wxString str = findText + wxT('%');
    BookInfo::MakeLower(str);

    FbCommonDatabase database;
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, str);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FillAuthors(result);
}

void FbAuthorList::FillAuthors(wxSQLite3ResultSet & result)
{
	Freeze();
	Clear();

    while (result.NextRow()) {
        int id = result.GetInt(wxT("id"));
        AuthorItem item;
        item.first  = result.GetString(wxT("first_name"));
        item.middle = result.GetString(wxT("middle_name"));
        item.last   = result.GetString(wxT("last_name"));
        Append(item.GetFullName(), new FbClientData(id));
    }

	Thaw();
}
