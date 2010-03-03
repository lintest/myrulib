#include "FbDatabase.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbGenres.h"

#define DB_DATABASE_VERSION 10
#define DB_CONFIG_VERSION 2

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

wxCriticalSection FbDatabase::sm_queue;

void FbDatabase::CreateFullText()
{
	if ( TableExists(wxT("fts_book")) ) return;

	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_auth"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_auth USING fts3"));

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_book"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_book USING fts3"));

	ExecuteUpdate(wxT("DROP TABLE IF EXISTS fts_seqn"));
	ExecuteUpdate(wxT("CREATE VIRTUAL TABLE fts_seqn USING fts3"));

	FbLowerFunction	lower;
	CreateFunction(wxT("LOW"), 1, lower);
	ExecuteUpdate(wxT("INSERT INTO fts_auth(docid, content) SELECT DISTINCT id, LOW(search_name) FROM authors"));
	ExecuteUpdate(wxT("INSERT INTO fts_book(docid, content) SELECT DISTINCT id, LOW(title) FROM books"));
	ExecuteUpdate(wxT("INSERT INTO fts_seqn(docid, content) SELECT DISTINCT id, LOW(value) FROM sequences"));

	trans.Commit();
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
	wxString sql = wxString::Format(wxT("INSERT INTO authors(id, letter, full_name) values(0, '#', '%s')"), strNobody.c_str());
	ExecuteUpdate(sql);
	ExecuteUpdate(wxT("CREATE INDEX author_id ON authors(id)"));
	ExecuteUpdate(wxT("CREATE INDEX author_letter ON authors(letter)"));
	ExecuteUpdate(wxT("CREATE INDEX author_name ON authors(search_name)"));

	/** TABLE books **/
	ExecuteUpdate(wxT("\
			CREATE TABLE books(\
				id integer not null,\
				id_author integer not null,\
				title text not null,\
				annotation text,\
				genres text,\
				deleted boolean,\
				id_archive integer,\
				file_name text,\
				file_size integer,\
				file_type varchar(20),\
				description text);\
		"));
	ExecuteUpdate(wxT("CREATE INDEX book_id ON books(id)"));
	ExecuteUpdate(wxT("CREATE INDEX book_author ON books(id_author)"));
	ExecuteUpdate(wxT("CREATE INDEX book_archive ON books(id_archive)"));

	/** TABLE archives **/
	ExecuteUpdate(wxT("\
			CREATE TABLE archives(\
				id integer primary key,\
				file_name text,\
				file_path text,\
				file_size integer,\
				file_count integer,\
				min_id_book integer,\
				max_id_book integer,\
				file_type varchar(20),\
				description text);\
		"));

	/** TABLE sequences **/
	ExecuteUpdate(wxT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null)"));
	ExecuteUpdate(wxT("CREATE INDEX sequences_name ON sequences(value)"));

	/** TABLE bookseq **/
	ExecuteUpdate(wxT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer)"));
	ExecuteUpdate(wxT("CREATE INDEX bookseq_book ON bookseq(id_book)"));
	ExecuteUpdate(wxT("CREATE INDEX bookseq_author ON bookseq(id_author)"));

	/** TABLE words **/
	ExecuteUpdate(wxT("CREATE TABLE words(word varchar(99), id_book integer not null, number integer)"));
	ExecuteUpdate(wxT("CREATE INDEX words_word ON words(word)"));

	/** TABLE params **/
	ExecuteUpdate(wxT("CREATE TABLE params(id integer primary key, value integer, text text)"));
	ExecuteUpdate(wxT("INSERT INTO params(id, text)  VALUES (1, 'My own Library')"));
	ExecuteUpdate(wxT("INSERT INTO params(id, value) VALUES (2, 1)"));

	trans.Commit();

	CreateFullText();
}

void FbMainDatabase::DoUpgrade(int version)
{
	switch (version) {

		case 2: {
			/** TABLE books **/
			ExecuteUpdate(wxT("ALTER TABLE books ADD sha1sum VARCHAR(27)"));
			ExecuteUpdate(wxT("CREATE INDEX books_sha1sum ON books(sha1sum)"));
			ExecuteUpdate(wxT("CREATE INDEX book_filesize ON books(file_size)"));
			/** TABLE zip_books, zip_files **/
			ExecuteUpdate(wxT("CREATE TABLE zip_books(book varchar(99), file integer)"));
			ExecuteUpdate(wxT("CREATE TABLE zip_files(file integer primary key, path text)"));
			ExecuteUpdate(wxT("CREATE INDEX zip_books_name ON zip_books(book)"));
		} break;

		case 3: {
			/** TABLE types **/
			ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99), command text, convert text)"));
			ExecuteUpdate(wxT("CREATE UNIQUE INDEX types_file_type ON types(file_type)"));
			ExecuteUpdate(wxT("DROP INDEX IF EXISTS book_file"));
			/** TABLE files **/
			ExecuteUpdate(wxT("CREATE TABLE files(id_book integer, id_archive integer, file_name text)"));
			ExecuteUpdate(wxT("CREATE INDEX files_book ON files(id_book)"));
		} break;

		case 4: {
			/** TABLE books **/
			ExecuteUpdate(wxT("ALTER TABLE books ADD file_path TEXT"));
			ExecuteUpdate(wxT("ALTER TABLE books ADD rating INTEGER"));
			ExecuteUpdate(wxT("DROP INDEX IF EXISTS books_sha1sum"));
			try { ExecuteUpdate(wxT("ALTER TABLE books ADD md5sum CHAR(32)")); } catch (...) {};
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS book_md5sum ON books(md5sum)"));
			/** TABLE files **/
			ExecuteUpdate(wxT("ALTER TABLE files ADD file_path TEXT"));
			/** TABLE comments **/
			ExecuteUpdate(wxT("CREATE TABLE comments(id integer primary key, id_book integer, rating integer, posted datetime, caption text, comment text)"));
			ExecuteUpdate(wxT("CREATE INDEX comments_book ON comments(id_book)"));
		} break;

		case 5: {
			ExecuteUpdate(wxT("DROP TABLE IF EXISTS types"));
			ExecuteUpdate(wxT("DROP TABLE IF EXISTS comments"));
			ExecuteUpdate(wxT("DROP TABLE IF EXISTS words"));
		} break;

		case 6: {
			/** TABLE books **/
			try { ExecuteUpdate(wxT("ALTER TABLE books ADD created INTEGER")); } catch (...) {};
		} break;

		case 7: {
			/** TABLE aliases **/
			ExecuteUpdate(wxT("CREATE TABLE IF NOT EXISTS aliases(id_author integer not null, id_alias integer not null);"));
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS aliases_author ON aliases(id_author);"));
			ExecuteUpdate(wxT("CREATE INDEX IF NOT EXISTS aliases_alias ON aliases(id_alias);"));
			try {
				ExecuteUpdate(wxT("ALTER TABLE authors ADD number INTEGER"));
				ExecuteUpdate(strUpdateAuthorCount);
			} catch (...) {};
		} break;

		case 8: {
			/** TABLE aliases **/
			ExecuteUpdate(wxT("CREATE INDEX bookseq_seq ON bookseq(id_seq)"));
			try {
				ExecuteUpdate(wxT("ALTER TABLE sequences ADD number INTEGER"));
				ExecuteUpdate(wxT("DELETE FROM sequences WHERE NOT EXISTS (SELECT id_seq FROM bookseq WHERE sequences.id=id_seq) OR id=0"));
				ExecuteUpdate(strUpdateSequenCount);
			} catch (...) {};
		} break;

		case 9: {
			/** TABLE books **/
			try {
				ExecuteUpdate(wxT("ALTER TABLE books ADD lang CHAR(2)"));
				ExecuteUpdate(wxT("ALTER TABLE books ADD year INTEGER"));
			} catch (...) {};
		} break;

	}
}

void FbLowerFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount == 1) {
		ctx.SetResult(Lower(ctx.GetString(0)));
	} else {
		ctx.SetResultError(wxString::Format(wxT("LOWER called with wrong number of arguments: %d."), argCount));
	}
}

void FbSearchFunction::Decompose(const wxString &text, wxArrayString &list)
{
	wxString str = Lower(text);
	int i = wxNOT_FOUND;
	do {
		str.Trim(false);
		i = str.find(wxT(' '));
		if (i == wxNOT_FOUND) break;
		list.Add( str.Left(i) );
		str = str.Mid(i);
	} while (true);
	str.Trim(true);
	if (!str.IsEmpty()) list.Add(str);
}

FbSearchFunction::FbSearchFunction(const wxString & input)
{
	Decompose(input, m_masks);
	wxString log = _("Search template") + COLON;
	size_t count = m_masks.Count();
	for (size_t i=0; i<count; i++) {
		log += wxString::Format(wxT("<%s> "), m_masks[i].c_str());
	}
	wxLogInfo(log);
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

void FbGenreFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argCount = ctx.GetArgCount();
	if (argCount != 1) {
		ctx.SetResultError(wxString::Format(_("GENRE called with wrong number of arguments: %d."), argCount));
		return;
	}
	ctx.SetResult( FbGenres::DecodeList( ctx.GetString(0) ) );
}

void FbDatabase::Open(const wxString& fileName, const wxString& key, int flags)
{
	try {
		wxSQLite3Database::Open(fileName, key, flags);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		throw e;
	}
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

wxString FbDatabase::GetText(const int param)
{
	const wxChar * table = param < 100 ? wxT("params") : wxT("config");

	wxString sql = wxString::Format( wxT("SELECT text FROM %s WHERE id=?"), table);
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, param);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if (result.NextRow())
		return result.GetString(0);
	else
		return wxEmptyString;
}

FbCommonDatabase::FbCommonDatabase() :FbDatabase()
{
	FbDatabase::Open(wxGetApp().GetAppData());
}

FbLocalDatabase::FbLocalDatabase() :FbDatabase()
{
	FbDatabase::Open(GetConfigName());
}

void FbMainDatabase::Open(const wxString& filename, const wxString& key, int flags)
{
	bool bExists = wxFileExists(filename);

	if (bExists)
		wxLogInfo(_("Open database") + COLON + filename);
	else {
	    wxString info = _("Create new database");
		wxLogInfo(info + COLON + filename);
		wxString msg = strProgramName + (wxString)wxT(" - ") + info + (wxString)wxT("\n") + filename;
		wxMessageBox(msg);
	}

	try {
		FbDatabase::Open(filename, key, flags);
		if (!bExists) CreateDatabase();
		UpgradeDatabase(DB_DATABASE_VERSION);
	}
	catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

const wxString & FbDatabase::GetConfigName()
{
	static wxString filename = FbStandardPaths().GetConfigFile();
	return filename;
}

void FbCommonDatabase::AttachConfig()
{
	wxString sql = wxT("ATTACH ? AS config");
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, GetConfigName());
	stmt.ExecuteUpdate();
}

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
	ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99), command text, convert text)"));
	ExecuteUpdate(wxT("CREATE UNIQUE INDEX types_file_type ON types(file_type)"));
	ExecuteUpdate(wxT("DROP INDEX IF EXISTS book_file"));

	/** TABLE comments **/
	ExecuteUpdate(wxT("CREATE TABLE comments(id integer primary key, md5sum CHAR(32), rating integer, posted datetime, caption text, comment text)"));
	ExecuteUpdate(wxT("CREATE INDEX comments_book ON comments(md5sum)"));

	/** TABLE folders **/
	ExecuteUpdate(wxT("CREATE TABLE folders(id integer primary key, value text not null)"));
	ExecuteUpdate(wxT("INSERT INTO folders(id,value) VALUES (-1, 'Лучшее')"));
	ExecuteUpdate(wxT("INSERT INTO folders(id,value) VALUES (-2, 'Прочее')"));

	/** TABLE favorites **/
	ExecuteUpdate(wxT("CREATE TABLE favorites(id_folder integer, md5sum CHAR(32))"));
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
	}
}

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
		wxLogInfo(_("Upgrade database to version %d"), version);
		wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);
		DoUpgrade(version);
		SetVersion(version);
		trans.Commit();
	}

	int old_version = GetVersion();
	if (old_version != new_version) {
	    wxString msg = _("Database version mismatch");
		wxMessageBox(msg, strProgramName, wxOK | wxICON_ERROR);
		wxLogError(msg);
		wxLogFatalError(_("Need a new version %d, but used the old %d."), new_version, old_version);
	}
}

wxString FbCommonDatabase::GetMd5(int id)
{
	try {
		wxString sql = wxT("SELECT md5sum FROM books WHERE id=?");
		wxSQLite3Statement stmt = PrepareStatement(sql);
		stmt.Bind(1, id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) return result.GetAsString(0);
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
	return wxEmptyString;
}
