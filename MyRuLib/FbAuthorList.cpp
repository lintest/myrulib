#include "FbAuthorList.h"
#include "FbClientData.h"
#include "ImpContext.h"
#include "FbManager.h"

void FbAuthorList::FillAuthorsChar(const wxChar & findLetter)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE letter=? ORDER BY search_name");

    FbCommonDatabase database;
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, (wxString)findLetter);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FillAuthors(result);
}

void FbAuthorList::FillAuthorsText(const wxString & findText)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE search_name like ? ORDER BY search_name");
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
        wxString name = result.GetString(wxT("full_name"));
        Append(name, new FbClientData(id));
    }

	Thaw();
}
