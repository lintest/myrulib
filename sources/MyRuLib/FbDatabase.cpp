#include "FbDatabase.h"
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbGenres.h"
#include "FbString.h"
#include <wx/tokenzr.h>
#include <sqlite3.h>

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
#ifdef wxHAVE_TCHAR_SUPPORT
	return wxStrcoll(text1, text2);
#else
	return text1.CmpNoCase(text2);
#endif
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

static int DatabaseThreadCallback(void * data)
{
	return data && ((FbThread*)data)->IsClosed();
}

void FbDatabase::JoinThread(FbThread * thread)
{
	sqlite3 * db = (sqlite3*) GetDatabaseHandle();
	if (db) sqlite3_progress_handler( db, 100, DatabaseThreadCallback, thread);
}

//-----------------------------------------------------------------------------
//  FbCommonDatabase
//-----------------------------------------------------------------------------

FbCommonDatabase::FbCommonDatabase()
{
	FbDatabase::Open(wxGetApp().GetLibFile());
	ExecuteUpdate(fbT("PRAGMA temp_store=2"));
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
	ExecuteUpdate(fbT("PRAGMA temp_store=2"));
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
	ExecuteUpdate(fbT("PRAGMA temp_store=2"));
	bool bExists = TableExists(GetMaster());

	wxString message = bExists ? _("Open database") : _("Create database");
	FbLogMessage(message, filename);

	if (!bExists) CreateDatabase();
	UpgradeDatabase(DB_DATABASE_VERSION);
}

void FbMainDatabase::CreateDatabase()
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	/** TABLE authors **/
	ExecuteUpdate(fbT("CREATE TABLE authors(id INTEGER PRIMARY KEY,letter,search_name,full_name,first_name,middle_name,last_name,newid,description)"));
	ExecuteUpdate(fbT("INSERT INTO authors(id, letter, full_name) values(0, '#', '(empty)')"));
	ExecuteUpdate(fbT("CREATE INDEX author_letter ON authors(letter)"));
	ExecuteUpdate(fbT("CREATE INDEX author_name ON authors(search_name)"));

	/** TABLE books **/
	ExecuteUpdate(fbT("CREATE TABLE books(id INTEGER,id_author INTEGER,title,annotation,genres,deleted,id_archive,file_name,file_size,file_type,description,PRIMARY KEY(id,id_author))"));
	ExecuteUpdate(fbT("CREATE INDEX book_author ON books(id_author)"));
	ExecuteUpdate(fbT("CREATE INDEX book_archive ON books(id_archive)"));

	/** TABLE archives **/
	ExecuteUpdate(fbT("CREATE TABLE archives(id INTEGER PRIMARY KEY,file_name,file_path,file_size,file_count)"));

	/** TABLE sequences **/
	ExecuteUpdate(fbT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null)"));
	ExecuteUpdate(fbT("CREATE INDEX sequences_name ON sequences(value)"));

	/** TABLE bookseq **/
	ExecuteUpdate(fbT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer, PRIMARY KEY(id_book, id_seq))"));
	ExecuteUpdate(fbT("CREATE INDEX bookseq_seq ON bookseq(id_seq)"));

	/** TABLE params **/
	ExecuteUpdate(fbT("CREATE TABLE params(id integer primary key, value integer, text text)"));
	ExecuteUpdate(fbT("INSERT INTO params(id, text)  VALUES (1, 'My own Library')"));
	ExecuteUpdate(fbT("INSERT INTO params(id, value) VALUES (2, 1)"));

	/** TABLE genres **/
	ExecuteUpdate(fbT("CREATE TABLE genres(id_book integer, id_genre CHAR(2), PRIMARY KEY(id_book, id_genre));"));
	ExecuteUpdate(fbT("CREATE INDEX genres_genre ON genres(id_genre);"));

	trans.Commit();

	CreateFullText();
}

void FbMainDatabase::DoUpgrade(int version)
{
	switch (version) {

		case 2: {
		} break;

		case 3: {
			ExecuteUpdate(fbT("CREATE TABLE IF NOT EXISTS files(id_book integer, id_archive integer, file_name text, file_path text)"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS files_book ON files(id_book)"));
		} break;

		case 4: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE books ADD file_path TEXT"));
		} break;

		case 5: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE books ADD md5sum CHAR(32)"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS book_md5sum ON books(md5sum)"));
		} break;

		case 6: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE books ADD created INTEGER"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS book_created ON books(created)"));
		} break;

		case 7: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE authors ADD number INTEGER"));
		} break;

		case 8: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE sequences ADD number INTEGER"));
		} break;

		case 9: {
			wxLogNull log;
			ExecuteUpdate(fbT("ALTER TABLE books ADD lang CHAR(2)"));
			ExecuteUpdate(fbT("ALTER TABLE books ADD year INTEGER"));
		} break;

		case 11: {
			ExecuteUpdate(fbT("CREATE TABLE IF NOT EXISTS types(file_type varchar(99) PRIMARY KEY, command text, convert text)"));
		} break;

		case 12: {
		} break;
	}
}

void FbMainDatabase::CreateFullText(bool force, FbThread * thread)
{
	if ( !force && TableExists(wxT("fts_book_content")) ) return;

	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	ExecuteUpdate(fbT("DROP TABLE IF EXISTS fts_auth"));
	ExecuteUpdate(fbT("CREATE VIRTUAL TABLE fts_auth USING fts3"));

	ExecuteUpdate(fbT("DROP TABLE IF EXISTS fts_book"));
	ExecuteUpdate(fbT("CREATE VIRTUAL TABLE fts_book USING fts3"));

	ExecuteUpdate(fbT("DROP TABLE IF EXISTS fts_seqn"));
	ExecuteUpdate(fbT("CREATE VIRTUAL TABLE fts_seqn USING fts3"));

	FbLowerFunction	lower;
	CreateFunction(wxT("LOW"), 1, lower);
	if (thread && thread->IsClosed()) return;

	ExecuteUpdate(fbT("INSERT INTO fts_auth(docid, content) SELECT DISTINCT id, LOW(full_name) FROM authors"));
	if (thread && thread->IsClosed()) return;

	ExecuteUpdate(fbT("INSERT INTO fts_book(docid, content) SELECT DISTINCT id, LOW(title) FROM books"));
	if (thread && thread->IsClosed()) return;

	ExecuteUpdate(fbT("INSERT INTO fts_seqn(docid, content) SELECT DISTINCT id, LOW(value) FROM sequences"));
	if (thread && thread->IsClosed()) return;

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
	ExecuteUpdate(fbT("PRAGMA temp_store=2"));
	if (!bExists) CreateDatabase();
	UpgradeDatabase(DB_CONFIG_VERSION);
}

void FbConfigDatabase::CreateDatabase()
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

	/** TABLE params **/
	ExecuteUpdate(fbT("CREATE TABLE config(id integer primary key, value integer, text text)"));
	ExecuteUpdate(fbT("INSERT INTO config(id, text)  VALUES (1, 'MyRuLib local config')"));
	ExecuteUpdate(fbT("INSERT INTO config(id, value) VALUES (2, 1)"));

	/** TABLE types **/
	ExecuteUpdate(fbT("CREATE TABLE types(file_type varchar(99) primary key, command text, convert text)"));

	/** TABLE comments **/
	ExecuteUpdate(fbT("CREATE TABLE comments(id integer primary key, md5sum CHAR(32), rating integer, posted datetime, caption text, comment text)"));
	ExecuteUpdate(fbT("CREATE INDEX comments_book ON comments(md5sum)"));

	/** TABLE folders **/
	ExecuteUpdate(fbT("CREATE TABLE folders(id integer primary key, value text not null)"));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value) VALUES (-1, '%s')"), _("The best")));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value) VALUES (-2, '%s')"), _("Other")));

	/** TABLE favorites **/
	ExecuteUpdate(fbT("CREATE TABLE favorites(md5sum CHAR(32), id_folder integer, PRIMARY KEY(md5sum, id_folder))"));
	ExecuteUpdate(fbT("CREATE INDEX favorites_folder ON favorites(id_folder)"));

	trans.Commit();
}

wxString FbTransl(const char * psz) 
{
	return FbString(psz).Translate();
}

void FbConfigDatabase::DoUpgrade(int version)
{
	switch (version) {
		case 2: {
			/** TABLE states **/
			ExecuteUpdate(fbT("CREATE TABLE states(md5sum CHAR(32) primary key, rating INTEGER, download INTEGER)"));
			ExecuteUpdate(fbT("CREATE INDEX states_rating ON states(rating)"));
		} break;
		case 3: {
			/** TABLE script **/
			ExecuteUpdate(fbT("CREATE TABLE script(id INTEGER PRIMARY KEY, name TEXT, text TEXT)"));
		} break;
		case 4: {
			/** TABLE script **/
			ExecuteUpdate(fbT("UPDATE states SET download = - 2 - download WHERE download>0"));
			ExecuteUpdate(fbT("UPDATE states SET download = 1 WHERE download=-2"));
			ExecuteUpdate(fbT("UPDATE states SET download = 101 WHERE download=-1"));
		} break;
	}
}
