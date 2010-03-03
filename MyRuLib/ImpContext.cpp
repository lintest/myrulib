#include "ImpContext.h"
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

void AuthorItem::Bind(wxSQLite3Statement &stmt, int param, const wxString &value)
{
	if (value.IsEmpty()) {
		const char * null = "";
		stmt.Bind(param, null);
	} else
		stmt.Bind(param, value);
}

int AuthorItem::Find(FbDatabase & database)
{
	wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? AND id<>? ORDER BY search_name");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	Bind(stmt, 1, first);
	Bind(stmt, 2, middle);
	Bind(stmt, 3, last);
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

	wxString search_name = Lower(full_name);
	search_name.Replace(strRusJO, strRusJE);

	wxString letter = Upper(full_name.Left(1));
	if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

	wxString sql_data;
	wxString sql_fts3;
	if (id) {
		sql_data = wxT("UPDATE authors SET letter=?, search_name=?, full_name=?, first_name=?, middle_name=?, last_name=? WHERE id=?");
		sql_fts3 = wxT("UPDATE fts_auth SET content=? WHERE docid=?");
	} else {
		id = - database.NewId(DB_NEW_AUTHOR);
		sql_data = wxT("INSERT INTO authors(letter, search_name, full_name, first_name, middle_name, last_name, id) VALUES(?,?,?,?,?,?,?)");
		sql_fts3 = wxT("INSERT INTO fts_auth(content, docid) VALUES(?,?)");
	}

	{
		wxSQLite3Statement stmt = database.PrepareStatement(sql_data);
		Bind(stmt, 1, letter);
		Bind(stmt, 2, search_name);
		Bind(stmt, 3, full_name);
		Bind(stmt, 4, first);
		Bind(stmt, 5, middle);
		Bind(stmt, 6, last);
		stmt.Bind(7, id);
		stmt.ExecuteUpdate();
	}

	{
		wxString content = Lower(search_name);
		wxSQLite3Statement stmt = database.PrepareStatement(sql_fts3);
		stmt.Bind(1, content);
		stmt.Bind(2, id);
		stmt.ExecuteUpdate();
	}

	return id;
}

int SequenceItem::Convert(FbDatabase & database)
{
	if (seqname.IsEmpty()) return 0;

	{
		wxString sql = wxT("SELECT id FROM sequences WHERE value=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, seqname);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return id = result.GetInt(0);
	}

	int id = - database.NewId(DB_NEW_SEQUENCE);
	{
		wxString sql = wxT("INSERT INTO sequences(value,id) VALUES (?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, seqname);
		stmt.Bind(2, id);
		stmt.ExecuteUpdate();
	}

	{
		wxString content = Lower(seqname);
		wxString sql = wxT("INSERT INTO fts_seqn(content, docid) VALUES(?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, content);
		stmt.Bind(2, id);
		stmt.ExecuteUpdate();
	}

	return id;
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
