#include "FbDatabase.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbGenres.h"
#include <wx/tokenzr.h>

#define DB_DATABASE_VERSION 12
#define DB_CONFIG_VERSION 4

wxString Lower(const wxString & input)
{
#if defined(__WIN32__)
	int len = input.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, input.c_str());
	CharLower(buf);
	wxString output = buf;
	delete [] buf;
	return output;
#else
	return input.Lower();
#endif
}

wxString Upper(const wxString & input)
{
#if defined(__WIN32__)
	int len = input.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, input.c_str());
	CharUpper(buf);
	wxString output = buf;
	delete [] buf;
	return output;
#else
	return input.Upper();
#endif
}

wxString & MakeLower(wxString & data)
{
#if defined(__WIN32__)
	int len = data.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, data.c_str());
	CharLower(buf);
	data = buf;
	delete [] buf;
#else
	data.MakeLower();
#endif
	return data;
}

wxString & MakeUpper(wxString & data)
{
#if defined(__WIN32__)
	int len = data.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, data.c_str());
	CharUpper(buf);
	data = buf;
	delete [] buf;
#else
	data.MakeUpper();
#endif
	return data;
}

//-----------------------------------------------------------------------------
//  FbLowerFunction
//-----------------------------------------------------------------------------

void FbLowerFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount == 1) {
		ctx.SetResult(Lower(ctx.GetString(0)));
	} else {
		ctx.SetResultError(wxString::Format(wxT("LOWER called with wrong number of arguments: %d."), argCount));
	}
}

//-----------------------------------------------------------------------------
//  FbAuthorFunction
//-----------------------------------------------------------------------------

void FbAuthorFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	if (ctx.GetArgCount() == 3) {
		wxString res = ctx.GetString(0).Trim(true);
		(res << wxT(' ') << ctx.GetString(1).Trim(false)).Trim(true);
		(res << wxT(' ') << ctx.GetString(2).Trim(false)).Trim(true);
		ctx.SetResult(res.Trim(false));
	}
}

//-----------------------------------------------------------------------------
//  FbLetterFunction
//-----------------------------------------------------------------------------

void FbLetterFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	if (ctx.GetArgCount() == 1) {
		wxString res = Upper(ctx.GetString(0).Left(1));
		if (res == wxChar(0x401)) res = wxChar(0x415);
		if (res.IsEmpty() || strAlphabet.Find(res) == wxNOT_FOUND) res = wxT('#');
		ctx.SetResult(res);
	}
}

//-----------------------------------------------------------------------------
//  FbIncrementFunction
//-----------------------------------------------------------------------------

void FbIncrementFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int id = ctx.GetArgCount()>0 ? ctx.GetInt(0) : 0;
	m_increment++;
	id += m_increment;
	ctx.SetResult(-id);
}

//-----------------------------------------------------------------------------
//  FbSearchFunction
//-----------------------------------------------------------------------------

void FbSearchFunction::Decompose(const wxString &text, wxArrayString &list)
{
	wxStringTokenizer tkz(text, wxT(" "), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) list.Add(tkz.GetNextToken());
}

FbSearchFunction::FbSearchFunction(const wxString & input)
{
	Decompose(input, m_masks);
	wxString log = _("Search template"); log << wxT(": ");
	size_t count = m_masks.Count();
	for (size_t i=0; i<count; i++) {
		log << wxString::Format(wxT("<%s> "), m_masks[i].c_str());
	}
	wxLogMessage(log);
}

void FbSearchFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(wxT("SEARCH called with wrong number of arguments: %d."), argCount));
		return;
	}
	wxString text = Lower(ctx.GetString(0));

	wxArrayString words;
	Decompose(text, words);

	size_t maskCount = m_masks.Count();
	size_t wordCount = words.Count();

	for (size_t i=0; i<maskCount; i++) {
		bool bNotFound = true;
		wxString mask = m_masks[i] + wxT("*");
		for (size_t j=0; j<wordCount; j++) {
			if ( words[j].Matches(mask) ) {
				bNotFound = false;
				break;
			}
		}
		if (bNotFound) {
			ctx.SetResult(false);
			return;
		}
	}
	ctx.SetResult(true);
}

bool FbSearchFunction::IsFullText(const wxString &text)
{
	return ( text.Find(wxT("*")) == wxNOT_FOUND ) && ( text.Find(wxT("?")) == wxNOT_FOUND );
}

wxString FbSearchFunction::AddAsterisk(const wxString &text)
{
	wxString str = Lower(text);
	wxString result;
	int i = wxNOT_FOUND;
	do {
		str.Trim(false);
		i = str.find(wxT(' '));
		if (i == wxNOT_FOUND) break;
		result += str.Left(i) + wxT("* ");
		str = str.Mid(i);
	} while (true);
	str.Trim(true);
	if (!str.IsEmpty()) result += str.Left(i) + wxT("*");
	return result;
}

//-----------------------------------------------------------------------------
//  FbAggregateFunction
//-----------------------------------------------------------------------------

void FbAggregateFunction::Aggregate(wxSQLite3FunctionContext& ctx)
{
	wxSortedArrayString** acc = (wxSortedArrayString **) ctx.GetAggregateStruct(sizeof (wxSortedArrayString **));
	if (*acc == NULL) *acc = new wxSortedArrayString ;
	for (int i = 0; i < ctx.GetArgCount(); i++) {
		wxString text = ctx.GetString(i);
		text.Trim(true).Trim(false);
		if (!text.IsEmpty()) (**acc).Add(text);
	}
}

void FbAggregateFunction::Finalize(wxSQLite3FunctionContext& ctx)
{
	wxSortedArrayString ** acc = (wxSortedArrayString **) ctx.GetAggregateStruct(sizeof (wxSortedArrayString **));

	if ((*acc) == NULL) return;

	wxString result;
	size_t iCount = (*acc)->Count();
	for (size_t i=0; i<iCount; i++) {
		wxString text = (**acc).Item(i);
		if (!result.IsEmpty()) result << wxT(", ");
		if (!text.IsEmpty()) result << text;
	}
	ctx.SetResult(result);

	delete *acc;
	*acc = 0;
}

//-----------------------------------------------------------------------------
//  wxSQLite3Collation
//-----------------------------------------------------------------------------

int FbCyrillicCollation::Compare(const wxString& text1, const wxString& text2)
{
	return wxStrcoll(text1, text2);
}

//-----------------------------------------------------------------------------
//  FbDatabase
//-----------------------------------------------------------------------------

wxCriticalSection FbDatabase::sm_queue;

FbCyrillicCollation FbDatabase::sm_collation;

const wxString & FbDatabase::GetConfigName()
{
	static wxString filename = FbStandardPaths().GetConfigFile();
	return filename;
}

wxString FbDatabase::GetConfigPath()
{
	return wxFileName(FbDatabase::GetConfigName()).GetPath();
}

int FbDatabase::NewId(const int iParam, int iIncrement)
{
	wxCriticalSectionLocker enter(sm_queue);

	const wchar_t * table = iParam < 100 ? wxT("params") : wxT("config");

	int iValue = 0;
	{
		wxString sql = wxString::Format(wxT("SELECT value FROM %s WHERE id=?"), table);
		wxSQLite3Statement stmt = PrepareStatement(sql);
		stmt.Bind(1, iParam);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) iValue = result.GetInt(0);
	}

	if (iIncrement) {
		iValue += iIncrement;
		wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(value, id) VALUES(?,?)"), table);
		wxSQLite3Statement stmt = PrepareStatement(sql);
		stmt.Bind(1, iValue);
		stmt.Bind(2, iParam);
		stmt.ExecuteUpdate();
	}

	return iValue;
}

wxString FbDatabase::GetText(int param)
{
	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	wxString sql = wxString::Format( wxT("SELECT text FROM %s WHERE id=?"), table);
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, param);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) return result.GetString(0);
	return wxEmptyString;
}

void FbDatabase::SetText(int param, const wxString & text)
{
	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(id,text) VALUES(?,?)"), table);
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, param);
	stmt.Bind(2, text);
	stmt.ExecuteUpdate();
}

void FbDatabase::AttachConfig()
{
	wxString sql = wxT("ATTACH ? AS config");
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, GetConfigName());
	stmt.ExecuteUpdate();
}

//-----------------------------------------------------------------------------
//  FbCommonDatabase
//-----------------------------------------------------------------------------

FbCommonDatabase::FbCommonDatabase()
{
	FbDatabase::Open(wxGetApp().GetLibFile());
	SetCollation(wxT("CYR"), &sm_collation);
}

wxString FbCommonDatabase::GetMd5(int id)
{
	wxString sql = wxT("SELECT md5sum FROM books WHERE id=?");
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow()) return result.GetAsString(0);
	return wxEmptyString;
}

//-----------------------------------------------------------------------------
//  FbLocalDatabase
//-----------------------------------------------------------------------------

FbLocalDatabase::FbLocalDatabase()
{
	FbDatabase::Open(GetConfigName());
	SetCollation(wxT("CYR"), &sm_collation);
}

//-----------------------------------------------------------------------------
//  FbMasterDatabase
//-----------------------------------------------------------------------------

int FbMasterDatabase::GetVersion()
{
	return ExecuteScalar(wxString::Format(wxT("SELECT value FROM %s WHERE id=2"), GetMaster().c_str()));
}

void FbMasterDatabase::SetVersion(int iValue)
{
	ExecuteUpdate(wxString::Format(wxT("INSERT OR REPLACE INTO %s(id, value) VALUES (2,%d)"), GetMaster().c_str(), iValue));
}

void FbMasterDatabase::UpgradeDatabase(int new_version)
{
	int version = GetVersion();

	while ( version < new_version ) {
		version++;
		wxLogVerbose(_("Upgrade database to version %d"), version);
		wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);
		DoUpgrade(version);
		SetVersion(version);
		trans.Commit();
	}

	int old_version = GetVersion();
	if (old_version != new_version) {
	    wxString msg = _("Database version mismatch");
		wxMessageBox(msg, MyRuLib::ProgramName(), wxOK | wxICON_ERROR);
		wxLogError(msg);
		wxLogFatalError(_("Need a new version %d, but used the old %d."), new_version, old_version);
	}
}

//-----------------------------------------------------------------------------
//  FbMainDatabase
//-----------------------------------------------------------------------------

void FbMainDatabase::Open(const wxString& filename, const wxString& key, int flags)
{
	FbDatabase::Open(filename, key, flags);
	bool bExists = TableExists(GetMaster());

	wxString message = bExists ? _("Open database") : _("Create database");
	FbLogMessage(message, filename);

	if (!bExists) CreateDatabase();
	UpgradeDatabase(DB_DATABASE_VERSION);
	CreateFullText();
}

void FbMainDatabase::CreateDatabase()
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	/** TABLE authors **/
	ExecuteUpdate(wxT("\
		CREATE TABLE authors(\
			id integer primary key,\
			letter char(1),\
			search_name varchar(255),\
			full_name varchar(255),\
			first_name varchar(128),\
			middle_name varchar(128),\
			last_name varchar(128),\
			newid integer,\
			description text);\
	"));
	ExecuteUpdate(wxT("INSERT INTO authors(id, letter, full_name) values(0, '#', '(empty)')"));
	ExecuteUpdate(wxT("CREATE INDEX author_letter ON authors(letter)"));
	ExecuteUpdate(wxT("CREATE INDEX author_name ON authors(search_name)"));

	/** TABLE books **/
	ExecuteUpdate(wxT("\
		CREATE TABLE books(\
			id integer not null,\
			id_author integer,\
			title text not null,\
			annotation text,\
			genres text,\
			deleted boolean,\
			id_archive integer,\
			file_name text,\
			file_size integer,\
			file_type varchar(20),\
			description text, \
			PRIMARY KEY(id, id_author));\
	"));
	ExecuteUpdate(wxT("CREATE INDEX book_author ON books(id_author)"));
	ExecuteUpdate(wxT("CREATE INDEX book_archive ON books(id_archive)"));

	/** TABLE archives **/
	ExecuteUpdate(wxT("\
		CREATE TABLE archives(\
			id integer primary key,\
			file_name text,\
			file_path text,\
			file_size integer,\
			file_count integer);\
	"));

	/** TABLE sequences **/
	ExecuteUpdate(wxT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null)"));
	ExecuteUpdate(wxT("CREATE INDEX sequences_name ON sequences(value)"));

	/** TABLE bookseq **/
	ExecuteUpdate(wxT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer, PRIMARY KEY(id_book, id_seq))"));
	ExecuteUpdate(wxT("CREATE INDEX bookseq_seq ON bookseq(id_seq)"));

	/** TABLE params **/
	ExecuteUpdate(wxT("CREATE TABLE params(id integer primary key, value integer, text text)"));
	ExecuteUpdate(wxT("INSERT INTO params(id, text)  VALUES (1, 'My own Library')"));
	ExecuteUpdate(wxT("INSERT INTO params(id, value) VALUES (2, 1)"));

	/** TABLE genres **/
	ExecuteUpdate(wxT("CREATE TABLE genres(id_book integer, id_genre CHAR(2), PRIMARY KEY(id_book, id_genre));"));
  	ExecuteUpdate(wxT("CREATE INDEX genres_genre ON genres(id_genre);"));

	trans.Commit();

	CreateFullText();
}

void FbMainDatabase::DoUpgrade(int version)
{
	switch (version) {

		case 2: {
		} break;

		case 3: {
			ExecuteUpdate(wxT("CREATE TABLE IF NOT EXISTS files(id_book integer, id_archive integer, file_name text, file_path text)"));
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS files_book ON files(id_book)"));
		} break;

		case 4: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE books ADD file_path TEXT"));
		} break;

		case 5: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE books ADD md5sum CHAR(32)"));
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS book_md5sum ON books(md5sum)"));
		} break;

		case 6: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE books ADD created INTEGER"));
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS book_created ON books(created)"));
		} break;

		case 7: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE authors ADD number INTEGER"));
		} break;

		case 8: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE sequences ADD number INTEGER"));
		} break;

		case 9: {
			wxLogNull log;
			ExecuteUpdate(wxT("ALTER TABLE books ADD lang CHAR(2)"));
			ExecuteUpdate(wxT("ALTER TABLE books ADD year INTEGER"));
		} break;

		case 11: {
			ExecuteUpdate(wxT("CREATE TABLE IF NOT EXISTS types(file_type varchar(99) PRIMARY KEY, command text, convert text)"));
		} break;

		case 12: {
			if (!TableExists(wxT("dates"))) {
				ExecuteUpdate(wxT("CREATE TABLE dates(id integer primary key, lib_min integer, lib_max integer, lib_num, usr_min integer, usr_max, usr_num integer)"));
				ExecuteUpdate(wxT("INSERT INTO dates SELECT created, SUM(min_lib), SUM(max_lib), SUM(num_lib), SUM(min_usr), SUM(max_usr), SUM(num_usr) FROM (SELECT created, min(id) AS min_lib, max(id) AS max_lib, COUNT(DISTINCT id) AS num_lib, 0 AS min_usr, 0 AS max_usr, 0 AS num_usr FROM books WHERE id>0 GROUP BY created UNION ALL SELECT created, 0, 0, 0, min(id), max(id), COUNT(DISTINCT id) FROM books WHERE id<0 GROUP BY created)GROUP BY created"));
			}
		} break;
	}
}

void FbMainDatabase::CreateFullText(bool force)
{
	if ( !force && TableExists(wxT("fts_book_content")) ) return;

	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_auth"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_auth USING fts3"));

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_book"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_book USING fts3"));

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_seqn"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_seqn USING fts3"));

	FbLowerFunction	lower;
	CreateFunction(wxT("LOW"), 1, lower);
	ExecuteUpdate(wxT("INSERT INTO fts_auth(docid, content) SELECT DISTINCT id, LOW(full_name) FROM authors"));
	ExecuteUpdate(wxT("INSERT INTO fts_book(docid, content) SELECT DISTINCT id, LOW(title) FROM books"));
	ExecuteUpdate(wxT("INSERT INTO fts_seqn(docid, content) SELECT DISTINCT id, LOW(value) FROM sequences"));

	trans.Commit();
}

//-----------------------------------------------------------------------------
//  FbConfigDatabase
//-----------------------------------------------------------------------------

void FbConfigDatabase::Open()
{
	wxString filename = GetConfigName();
	bool bExists = wxFileExists(filename);
	FbDatabase::Open(filename, wxEmptyString, WXSQLITE_OPEN_READWRITE | WXSQLITE_OPEN_CREATE | WXSQLITE_OPEN_FULLMUTEX);
	if (!bExists) CreateDatabase();
	UpgradeDatabase(DB_CONFIG_VERSION);
}

void FbConfigDatabase::CreateDatabase()
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	/** TABLE params **/
	ExecuteUpdate(wxT("CREATE TABLE config(id integer primary key, value integer, text text)"));
	ExecuteUpdate(wxT("INSERT INTO config(id, text)  VALUES (1, 'MyRuLib local config')"));
	ExecuteUpdate(wxT("INSERT INTO config(id, value) VALUES (2, 1)"));

	/** TABLE types **/
	ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99) primary key, command text, convert text)"));

	/** TABLE comments **/
	ExecuteUpdate(wxT("CREATE TABLE comments(id integer primary key, md5sum CHAR(32), rating integer, posted datetime, caption text, comment text)"));
	ExecuteUpdate(wxT("CREATE INDEX comments_book ON comments(md5sum)"));

	/** TABLE folders **/
	ExecuteUpdate(wxT("CREATE TABLE folders(id integer primary key, value text not null)"));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value) VALUES (-1, '%s')"), _("The best")));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value) VALUES (-2, '%s')"), _("Other")));

	/** TABLE favorites **/
	ExecuteUpdate(wxT("CREATE TABLE favorites(md5sum CHAR(32), id_folder integer, PRIMARY KEY(md5sum, id_folder))"));
	ExecuteUpdate(wxT("CREATE INDEX favorites_folder ON favorites(id_folder)"));

	trans.Commit();
}

void FbConfigDatabase::DoUpgrade(int version)
{
	switch (version) {
		case 2: {
			/** TABLE states **/
			ExecuteUpdate(wxT("CREATE TABLE states(md5sum CHAR(32) primary key, rating INTEGER, download INTEGER)"));
			ExecuteUpdate(wxT("CREATE INDEX states_rating ON states(rating)"));
		} break;
		case 3: {
			/** TABLE script **/
			ExecuteUpdate(wxT("CREATE TABLE script(id INTEGER PRIMARY KEY, name TEXT, text TEXT)"));
		} break;
		case 4: {
			/** TABLE script **/
			ExecuteUpdate(wxT("UPDATE states SET download = - 2 - download WHERE download>0"));
			ExecuteUpdate(wxT("UPDATE states SET download = 1 WHERE download=-2"));
			ExecuteUpdate(wxT("UPDATE states SET download = 101 WHERE download=-1"));
		} break;
	}
}

