#include "FbAuthorList.h"
#include "FbClientData.h"
#include "ImpContext.h"
#include "FbManager.h"

FbAuthorList::FbAuthorList(wxWindow * parent, wxWindowID id)
	:FbTreeListCtrl(parent, id, wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER)
{
	this->AddColumn(_("Автор"), 10, wxALIGN_LEFT);
}

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

void FbAuthorList::FillAuthorsCode(const int code)
{
	wxString sql = wxT("SELECT id, full_name FROM authors WHERE id=?");
    FbCommonDatabase database;
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, code);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

	FillAuthors(result);
}

void FbAuthorList::FillAuthors(wxSQLite3ResultSet & result)
{
	FbTreeListUpdater(this);

	DeleteRoot();
	wxTreeItemId root = AddRoot(wxEmptyString);

    while (result.NextRow()) {
        int id = result.GetInt(wxT("id"));
        FbAuthorData * data = new FbAuthorData(id);
        wxString name = result.GetString(wxT("full_name"));
		wxTreeItemId item = AppendItem(root, name, -1, -1, data);
    }

    Expand(root);
}
