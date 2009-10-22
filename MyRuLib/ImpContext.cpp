#include "ImpContext.h"
#include "FbManager.h"
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

int AuthorItem::FindAuthor(FbDatabase & database)
{
	{
		wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? ORDER BY search_name");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, first);
		stmt.Bind(2, middle);
		stmt.Bind(3, last);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetInt(0);
	}

	wxString search_name = GetFullName();
	if (search_name.IsEmpty()) return 0;
	BookInfo::MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

	wxString letter = search_name.Left(1);
	BookInfo::MakeUpper(letter);
	if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

	int newId = - database.NewId(DB_NEW_AUTHOR);
	{
		wxString sql = wxT("INSERT INTO authors(id, letter, search_name, full_name, first_name, middle_name, last_name) VALUES(?,?,?,?,?,?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, newId);
		stmt.Bind(2, letter);
		stmt.Bind(3, search_name);
		stmt.Bind(4, GetFullName());
		stmt.Bind(5, first);
		stmt.Bind(6, middle);
		stmt.Bind(7, last);
		stmt.ExecuteUpdate();
	}
	return newId;
}

int SequenceItem::FindSequence(FbDatabase & database)
{
	if (seqname.IsEmpty()) return 0;

	{
		wxString sql = wxT("SELECT id FROM sequences WHERE value=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, seqname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetInt(0);
	}

	int newId = - database.NewId(DB_NEW_SEQUENCE);
	{
		wxString sql = wxT("INSERT INTO sequences(value,id) VALUES (?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, seqname);
		stmt.Bind(2, newId);
		stmt.ExecuteUpdate();
	}
	return newId;
}

