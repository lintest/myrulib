#include "FbImportCtx.h"
#include "FbConst.h"

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(SequenceArray);

WX_DEFINE_OBJARRAY(AuthorArray);

wxString AuthorItem::GetFullName()
{
	wxString result = last;
	if (!first.IsEmpty()) result << wxT(' ') << first;
	if (!middle.IsEmpty()) result << wxT(' ') << middle;
	return result.Trim(false).Trim(true);
}

void AuthorItem::Convert(FbDatabase & database)
{
	if (!Find(database)) Save(database);
}

void AuthorItem::Bind(wxSQLite3Statement &stmt, int param, const wxString &value)
{
	if (value.IsEmpty()) {
		const char empty = 0;
		stmt.Bind(param, &empty);
	} else
		stmt.Bind(param, value);
}

int AuthorItem::Find(FbDatabase & database)
{
	wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? AND id<>?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	Bind(stmt, 1, first);
	Bind(stmt, 2, middle);
	Bind(stmt, 3, last);
	stmt.Bind(4, m_id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return m_id = result.NextRow() ? result.GetInt(0) : 0;
}

int AuthorItem::Load(FbDatabase & database)
{
	wxString sql = wxT("SELECT first_name, middle_name, last_name FROM authors WHERE id=?");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if ( result.NextRow() ) {
		first  = result.GetString(0);
		middle = result.GetString(1);
		last   = result.GetString(2);
		return m_id;
	}
	return 0;
}

int AuthorItem::Save(FbDatabase & database)
{
	wxString full_name = GetFullName();
	if (full_name.IsEmpty()) { return m_id = 0; }

	wxString sql_data;
	wxString sql_fts3;
	if (m_id) {
		sql_data = wxT("UPDATE authors SET letter=LTTR(?), full_name=?, first_name=?, middle_name=?, last_name=? WHERE id=?");
		sql_fts3 = wxT("UPDATE fts_auth SET content=LOW(?) WHERE docid=?");
	} else {
		m_id = - database.NewId(DB_NEW_AUTHOR);
		sql_data = wxT("INSERT INTO authors(letter, full_name, first_name, middle_name, last_name, id) VALUES(LTTR(?),?,?,?,?,?)");
		sql_fts3 = wxT("INSERT INTO fts_auth(content, docid) VALUES(LOW(?),?)");
	}

	{
		wxSQLite3Statement stmt = database.PrepareStatement(sql_data);
		Bind(stmt, 1, full_name);
		Bind(stmt, 2, full_name);
		Bind(stmt, 3, first);
		Bind(stmt, 4, middle);
		Bind(stmt, 5, last);
		stmt.Bind(6, m_id);
		stmt.ExecuteUpdate();
	}

	{
		wxSQLite3Statement stmt = database.PrepareStatement(sql_fts3);
		stmt.Bind(1, full_name);
		stmt.Bind(2, m_id);
		stmt.ExecuteUpdate();
	}

	return m_id;
}

int SequenceItem::Convert(FbDatabase & database)
{
	if (m_name.IsEmpty()) return m_id = 0;

	if (m_id = database.Int(m_name, wxT("SELECT id FROM sequences WHERE value=?"))) return m_id;

	m_id = - database.NewId(DB_NEW_SEQUENCE);
	{
		wxString sql = wxT("INSERT INTO sequences(value,id) VALUES (?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_name);
		stmt.Bind(2, m_id);
		stmt.ExecuteUpdate();
	}

	{
		wxString sql = wxT("INSERT INTO fts_seqn(content, docid) VALUES(LOW(?),?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_name);
		stmt.Bind(2, m_id);
		stmt.ExecuteUpdate();
	}

	return m_id;
}

SequenceItem::SequenceItem(const FbStringHash &atts)
	: m_id(0), m_number(0)
{
	FbStringHash::const_iterator it;
	for( it = atts.begin(); it != atts.end(); ++it ) {
		wxString attr = it->first;
		wxString text = it->second;
		if (attr == wxT("name")) m_name = text; else 
		if (attr == wxT("number")) text.ToLong(&m_number);
	}
}
