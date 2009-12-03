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

void AuthorItem::Convert(FbDatabase & database)
{
	if (!Find(database)) Save(database);
}

int AuthorItem::Find(FbDatabase & database)
{
	wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? AND id<>? ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, first);
	stmt.Bind(2, middle);
	stmt.Bind(3, last);
	stmt.Bind(4, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return id = result.NextRow() ? result.GetInt(0) : 0;
}

int AuthorItem::Load(FbDatabase & database)
{
	wxString sql = wxT("SELECT first_name, middle_name, last_name FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if ( result.NextRow() ) {
		first  = result.GetString(0);
		middle = result.GetString(1);
		last   = result.GetString(2);
		return id;
	}
	return 0;
}

int AuthorItem::Save(FbDatabase & database)
{
	wxString full_name = GetFullName();
	if (full_name.IsEmpty()) { return id = 0; }

	wxString search_name = full_name;
	BookInfo::MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

	wxString letter = search_name.Left(1);
	BookInfo::MakeUpper(letter);
	if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

	wxString sql;
	if (id) {
		sql = wxT("UPDATE authors SET letter=?, search_name=?, full_name=?, first_name=?, middle_name=?, last_name=? WHERE id=?");
	} else {
		id = - database.NewId(DB_NEW_AUTHOR);
		sql = wxT("INSERT INTO authors(letter, search_name, full_name, first_name, middle_name, last_name, id) VALUES(?,?,?,?,?,?,?)");
	}

	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, letter);
	stmt.Bind(2, search_name);
	stmt.Bind(3, full_name);
	stmt.Bind(4, first);
	stmt.Bind(5, middle);
	stmt.Bind(6, last);
	stmt.Bind(7, id);
	stmt.ExecuteUpdate();

	return id;
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

SequenceItem::SequenceItem(const XML_Char **atts)
	: id(0), number(0)
{
	const XML_Char **a = atts;
	while (*a) {
		wxString attr = ParsingContext::CharToLower(a[0]).Trim(false).Trim(true); a++;
		wxString text = ParsingContext::CharToString(a[0]).Trim(false).Trim(true); a++;
		if (attr == wxT("name")) seqname = text;
		else if (attr == wxT("number")) text.ToLong(&number);
	}
}
