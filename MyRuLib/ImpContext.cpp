#include "ImpContext.h"
#include "FbManager.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include "FbConst.h"

WX_DEFINE_OBJARRAY(SequenceArray);

WX_DEFINE_OBJARRAY(AuthorArray);

wxString AuthorItem::GetFullName()
{
    wxString result = last;
    if (!first.IsEmpty()) result += (wxT(" ") + first);
    if (!middle.IsEmpty()) result += (wxT(" ") + middle);
    return result.Trim(false).Trim(true);
}

int AuthorItem::FindAuthor(AuthorItem &author)
{
    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    wxSQLite3Database & database = wxGetApp().GetDatabase();

    {
        wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? ORDER BY search_name");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, author.first);
        stmt.Bind(2, author.middle);
        stmt.Bind(3, author.last);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return result.GetInt(0);
    }

	wxString search_name = author.GetFullName();
	if (search_name.IsEmpty()) return 0;
	BookInfo::MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxString letter = search_name.Left(1);
    BookInfo::MakeUpper(letter);
    if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

    int newId = - BookInfo::NewId(DB_NEW_AUTHOR);
    {
        wxString sql = wxT("INSERT INTO authors(id, letter, search_name, full_name, first_name, middle_name, last_name VALUES(?,?,?,?,?,?,?)");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, newId);
        stmt.Bind(2, letter);
        stmt.Bind(3, search_name);
        stmt.Bind(4, author.GetFullName());
        stmt.Bind(5, author.first);
        stmt.Bind(6, author.middle);
        stmt.Bind(7, author.last);
        stmt.ExecuteUpdate();
    }
    return newId;
}

int SequenceItem::FindSequence(wxString &value)
{
	if (value.IsEmpty()) return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);
    wxSQLite3Database & database = wxGetApp().GetDatabase();

    {
        wxString sql = wxT("SELECT id FROM sequences WHERE value=?");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, value);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) return result.GetInt(0);
    }

    int newId = - BookInfo::NewId(DB_NEW_SEQUENCE);
    {
        wxString sql = wxT("INSERT INTO sequences(value,id) VALUES (?,?)");
        wxSQLite3Statement stmt = database.PrepareStatement(sql);
        stmt.Bind(1, value);
        stmt.Bind(2, newId);
        stmt.ExecuteUpdate();
    }
	return newId;
}

