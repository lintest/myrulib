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
#ifdef __WXMSW__
	wxWCharBuffer buf = input.c_str();
	CharLower(buf.data());
	return buf;
#else
	return input.Lower();
#endif
}

wxString Upper(const wxString & input)
{
#ifdef __WXMSW__
	wxWCharBuffer buf = input.c_str();
	CharUpper(buf.data());
	return buf;
#else
	return input.Upper();
#endif
}

wxString & MakeLower(wxString & data)
{
#ifdef __WXMSW__
	wxWCharBuffer buf = data.c_str();
	CharLower(buf.data());
	return data = buf;
#else
	return data.MakeLower();
#endif
}

wxString & MakeUpper(wxString & data)
{
#ifdef __WXMSW__
	wxWCharBuffer buf = data.c_str();
	CharUpper(buf.data());
	return data = buf;
#else
	return data.MakeUpper();
#endif
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

#ifdef SQLITE_ENABLE_ICU

#include "unicode/uchar.h"

static bool IsAlpha(wxChar ch)
{
	return u_isalpha(ch);
}

static bool IsNumeric(wxChar ch)
{
	return u_isdigit(ch);
}

bool IsAlphaNumeric(wxChar ch)
{
	return u_isalnum(ch);
}

#else  // SQLITE_ENABLE_ICU

#ifdef __WXMSW__

static bool IsAlpha(wxChar ch)
{
	return IsCharAlpha(ch);
}

static bool IsNumeric(wxChar ch)
{
	return IsCharAlphaNumeric(ch) && !IsCharAlphaNumeric(ch);
}

bool IsAlphaNumeric(wxChar ch)
{
	return IsCharAlphaNumeric(ch);
}

#else  // __WXMSW__

static bool IsAlpha(wxChar ch)
{
	return ch >= 0x41;
}

static bool IsNumeric(wxChar ch)
{
	return 0x30 <= ch && ch <= 0x39;
}

bool IsAlphaNumeric(wxChar ch)
{
	wxString forbidden = fbT("*?\\/:\"<>.,|") << (wxChar)0xAB << (wxChar)0xBB;
	return ch >= 0x30 && (forbidden.Find(ch) == wxNOT_FOUND);
}

#endif // __WXMSW__

#endif // SQLITE_ENABLE_ICU

wxString Letter(const wxString & text)
{
	size_t count = text.Len();
	for (size_t i = 0; i < count; i++) {
		wxChar ch = text[i];
		if (IsAlpha(ch)) {
			wxString res = Upper(ch);
			if (res == wxChar(0x401)) res = wxChar(0x415);
			return res;
		} else if (IsNumeric(ch)) {
			return wxT('#');
		}
	}
	return wxT('#');
}

void FbLetterFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	if (ctx.GetArgCount() == 1) {
		ctx.SetResult(Letter(ctx.GetString(0)));
	}
}

//-----------------------------------------------------------------------------
//  FbLowerFunction
//-----------------------------------------------------------------------------

void FbLowerFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argc = ctx.GetArgCount();
	if (argc == 1) {
		wxString str = Lower(ctx.GetString(0));
		size_t len = str.Len();
		for (size_t i = 0; i < len; i++) {
			if (str[i] == 0x0451) str[i] = 0x0435;
			#ifndef SQLITE_ENABLE_ICU
			if (!IsAlpha(str[i])) str[i] = 0x20;
			#endif
		}
		ctx.SetResult(str);
	} else {
		ctx.SetResultError(wxString::Format(fbT("Wrong LOWER argc: %d."), argc));
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

FbSearchFunction::FbSearchFunction(const wxString & input)
{
	FbLogMessage(_("Search template"), input);
	wxStringTokenizer tkz(Lower(input), wxT(' '), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		m_masks.Add(tkz.GetNextToken());
	}
}

void FbSearchFunction::Execute(wxSQLite3FunctionContext& ctx)
{
	int argc = ctx.GetArgCount();
	if (argc != 1) {
		ctx.SetResultError(wxString::Format(fbT("Wrong SEARCH argc: %d."), argc));
		return;
	}

	wxString input = ctx.GetString(0);
	wxArrayString words;
	wxStringTokenizer tkz(Lower(input), wxT(' '), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		words.Add(tkz.GetNextToken());
	}

	size_t count = m_masks.Count();
	for (size_t i = 0; i < count; i++) {
		bool not_found = true;
		size_t count = words.Count();
		for (size_t j = 0; j < count; j++) {
			if ( words[j].Matches(m_masks[i]) ) {
				not_found = false;
				break;
			}
		}
		if (not_found) {
			ctx.SetResult(false);
			return;
		}
	}
	ctx.SetResult(true);
}

bool FbSearchFunction::IsFullText(const wxString &text)
{
	return ( text.Find(wxT('*')) == wxNOT_FOUND ) && ( text.Find(wxT('?')) == wxNOT_FOUND );
}

//-----------------------------------------------------------------------------
//  FbSQLite3Statement
//-----------------------------------------------------------------------------

void FbSQLite3Statement::FTS(int index, const wxString& value)
{
	wxString result;
	wxString keywords = wxT("OR AND NOT");
	wxStringTokenizer tkz(value, wxT(' '), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		if (!result.IsEmpty()) result << wxT(' ');
		wxString word = tkz.GetNextToken();
		if (keywords.Find(word) != wxNOT_FOUND) {
		} else if (word.Left(4) == wxT("NEAR")) {
		} else {
			word = Lower(word);
			size_t len = word.Len();
			for (size_t i = 0; i < len; i++) {
				if (word[i] == 0x0451) word[i] = 0x0435;
			}
			if (word.Right(1) == wxT('"')) {
				word.insert(word.Len() - 1, wxT('*'));
			} else {
				word << wxT('*');
			}
		}
		result << word;
	}
	wxSQLite3Statement::Bind(index, result);
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


#ifdef SQLITE_ENABLE_ICU

#include <unicode/coll.h>

FbCyrillicCollation::FbCyrillicCollation()
	: m_collator(NULL)
{
	UErrorCode status = U_ZERO_ERROR;
	icu::Collator * collator = icu::Collator::createInstance(icu::Locale("ru", "RU"), status);
	if (U_FAILURE(status)) {
		delete collator;
	} else {
		m_collator = collator;
	}
}

FbCyrillicCollation::~FbCyrillicCollation()
{
	icu::Collator * collator = (icu::Collator*) m_collator;
	if (collator) delete collator;
}

int FbCyrillicCollation::Compare(const wxString& text1, const wxString& text2)
{
	icu::Collator * collator = (icu::Collator*) m_collator;
	if (collator) {
		UErrorCode status = U_ZERO_ERROR;
		wxCharBuffer buf1 = text1.mb_str();
		wxCharBuffer buf2 = text2.mb_str();
		return collator->compareUTF8(buf1.data(), buf2.data(), status);
	} else {
		return text1.CmpNoCase(text2);
	}
}

int FbCompare(const wxString& text1, const wxString& text2)
{
	return FbDatabase::sm_collation.Compare(text1, text2);
}

#else // SQLITE_ENABLE_ICU

FbCyrillicCollation::FbCyrillicCollation()
	: m_collator(NULL)
{
}

FbCyrillicCollation::~FbCyrillicCollation()
{
}

#ifdef __WXMSW__

int FbCompare(const wxString& text1, const wxString& text2)
{
   LCID locale = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA), SORT_DEFAULT);
   return CompareString(locale, NORM_IGNORECASE, text1.c_str(), text1.Len(), text2.c_str(), text2.Len()) - 2;
}

#else  // __WXMSW__

int FbCompare(const wxString& text1, const wxString& text2)
{
#ifdef wxHAVE_TCHAR_SUPPORT
	return wxStrcoll(text1, text2);
#else
	return text1.CmpNoCase(text2);
#endif
}

#endif // __WXMSW__

int FbCyrillicCollation::Compare(const wxString& text1, const wxString& text2)
{
	return FbCompare(text1, text2);
}

#endif // SQLITE_ENABLE_ICU

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
		wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(value,id)VALUES(?,?)"), table);
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
	return result.NextRow() ? result.GetString(0) : wxString();
}

void FbDatabase::SetText(int param, const wxString & text)
{
	const wxChar * table = param < 100 ? wxT("params") : wxT("config");
	wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(id,text)VALUES(?,?)"), table);
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, param);
	stmt.Bind(2, text);
	stmt.ExecuteUpdate();
}

void FbDatabase::AttachCommon()
{
	SetCollation(wxT("CYR"), &sm_collation);
	wxString sql = wxT("ATTACH ? AS config");
	wxSQLite3Statement stmt = PrepareStatement(sql);
	stmt.Bind(1, wxGetApp().GetLibFile());
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

wxString FbDatabase::Str(int id, const wxString & sql, const wxString & null)
{
	wxSQLite3Statement stmt = PrepareStatement(sql + wxT(" LIMIT 1"));
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetString(0, null) : null;
}

wxString FbDatabase::Str(const wxString & id, const wxString & sql, const wxString & null)
{
	wxSQLite3Statement stmt = PrepareStatement(sql + wxT(" LIMIT 1"));
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetString(0, null) : null;
}

int FbDatabase::Int(int id, const wxString & sql, int null)
{
	wxSQLite3Statement stmt = PrepareStatement(sql + wxT(" LIMIT 1"));
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0, null) : null;
}

int FbDatabase::Int(const wxString & id, const wxString & sql, int null)
{
	wxSQLite3Statement stmt = PrepareStatement(sql + wxT(" LIMIT 1"));
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	return result.NextRow() ? result.GetInt(0, null) : null;
}

void FbDatabase::InitFunctions()
{
	CreateFunction(wxT("LTTR"), 1, m_letter_func);
	CreateFunction(wxT("LOW"), 1, m_lower_func);
}

//-----------------------------------------------------------------------------
//  FbCommonDatabase
//-----------------------------------------------------------------------------

FbCommonDatabase::FbCommonDatabase()
{
	FbDatabase::Open(wxGetApp().GetLibFile());
	ExecuteUpdate(fbT("PRAGMA temp_store=2"));
#ifdef SQLITE_ENABLE_ICU
	if (sqlite3_compileoption_used("SQLITE_ENABLE_ICU")) {
		ExecuteUpdate(fbT("SELECT icu_load_collation('ru_RU','CYR')"));
	} else
#endif
	SetCollation(wxT("CYR"), &sm_collation);
	InitFunctions();
}

wxString FbCommonDatabase::GetMd5(int id)
{
	return Str(id, wxT("SELECT md5sum FROM books WHERE id=?"));
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
	ExecuteUpdate(wxString::Format(wxT("INSERT OR REPLACE INTO %s(id,value)VALUES(2,%d)"), GetMaster().c_str(), iValue));
}

void FbMasterDatabase::UpgradeDatabase(int new_version)
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_IMMEDIATE);
	int version = GetVersion();
	while ( version < new_version ) {
		version++;
		wxLogVerbose(_("Upgrade database to version %d"), version);
		DoUpgrade(version);
		SetVersion(version);
	}
	trans.Commit();

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
	InitFunctions();

	wxString message = bExists ? _("Open database") : _("Create database");
	FbLogMessage(message, filename);

	if (!bExists) CreateDatabase();
	UpgradeDatabase(DB_DATABASE_VERSION);
}

void FbMainDatabase::CreateDatabase()
{
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_IMMEDIATE);

	/** TABLE authors **/
	ExecuteUpdate(fbT("CREATE TABLE authors(id INTEGER PRIMARY KEY,letter,search_name,full_name,first_name,middle_name,last_name,newid,description)"));
	ExecuteUpdate(fbT("INSERT INTO authors(id,letter,full_name)values(0,'#','(empty)')"));
	ExecuteUpdate(fbT("CREATE INDEX author_letter ON authors(letter)"));
	ExecuteUpdate(fbT("CREATE INDEX author_name ON authors(search_name)"));

	/** TABLE books **/
	ExecuteUpdate(fbT("CREATE TABLE books(id INTEGER,id_author INTEGER,title,annotation,genres,deleted,id_archive,file_name,file_size,file_type,description,PRIMARY KEY(id,id_author))"));
	ExecuteUpdate(fbT("CREATE INDEX book_author ON books(id_author)"));
	ExecuteUpdate(fbT("CREATE INDEX book_archive ON books(id_archive)"));

	/** TABLE archives **/
	ExecuteUpdate(fbT("CREATE TABLE archives(id INTEGER PRIMARY KEY,file_name,file_path,file_size,file_count)"));

	/** TABLE sequences **/
	ExecuteUpdate(fbT("CREATE TABLE sequences(id INTEGER PRIMARY KEY,value)"));
	ExecuteUpdate(fbT("CREATE INDEX sequences_name ON sequences(value)"));

	/** TABLE bookseq **/
	ExecuteUpdate(fbT("CREATE TABLE bookseq(id_book INTEGER, id_seq INTEGER,number INTEGER,level INTEGER,id_author INTEGER,PRIMARY KEY(id_book,id_seq))"));
	ExecuteUpdate(fbT("CREATE INDEX bookseq_seq ON bookseq(id_seq)"));

	/** TABLE params **/
	ExecuteUpdate(fbT("CREATE TABLE params(id INTEGER PRIMARY KEY,value INTEGER,text)"));
	ExecuteUpdate(fbT("INSERT INTO params(id,text)VALUES (1,'My own Library')"));
	ExecuteUpdate(fbT("INSERT INTO params(id,value)VALUES (2,1)"));

	/** TABLE genres **/
	ExecuteUpdate(fbT("CREATE TABLE genres(id_book integer,id_genre CHAR(2),PRIMARY KEY(id_book,id_genre))"));
	ExecuteUpdate(fbT("CREATE INDEX genres_genre ON genres(id_genre)"));

	trans.Commit();

	CreateFullText();
}

void FbMainDatabase::DoUpgrade(int version)
{
	wxLogNull log;
   	switch (version) {

		case 2: {
		} break;

		case 3: {
			ExecuteUpdate(fbT("CREATE TABLE IF NOT EXISTS files(id_book INTEGER,id_archive INTEGER,file_name,file_path)"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS files_book ON files(id_book)"));
		} break;

		case 4: {
			ExecuteUpdate(fbT("ALTER TABLE books ADD file_path TEXT"));
		} break;

		case 5: {
			ExecuteUpdate(fbT("ALTER TABLE books ADD md5sum CHAR(32)"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS book_md5sum ON books(md5sum)"));
		} break;

		case 6: {
			ExecuteUpdate(fbT("ALTER TABLE books ADD created INTEGER"));
			ExecuteUpdate(fbT("CREATE INDEX IF NOT EXISTS book_created ON books(created)"));
		} break;

		case 7: {
			ExecuteUpdate(fbT("ALTER TABLE authors ADD number INTEGER"));
		} break;

		case 8: {
			ExecuteUpdate(fbT("ALTER TABLE sequences ADD number INTEGER"));
		} break;

		case 9: {
			ExecuteUpdate(fbT("ALTER TABLE books ADD lang CHAR(2)"));
			ExecuteUpdate(fbT("ALTER TABLE books ADD year INTEGER"));
		} break;

		case 11: {
			ExecuteUpdate(fbT("CREATE TABLE IF NOT EXISTS types(file_type varchar(99) PRIMARY KEY,command,convert)"));
		} break;

		case 12: {
		} break;
	}
}

void FbMainDatabase::CreateTableFTS(const wxString & name, const wxString & table, const wxString & field)
{
	ExecuteUpdate(wxString::Format(wxT("DROP TABLE IF EXISTS fts_%s"), name.c_str()));
	wxString sql = wxString::Format(wxT("CREATE VIRTUAL TABLE fts_%s USING fts3"), name.c_str());
	#ifdef SQLITE_ENABLE_ICU
		sql << wxT("(tokenize=icu ru_RU)");
	#else
		sql << wxT("(tokenize=porter)");
	#endif
	ExecuteUpdate(sql);
	sql = wxString::Format(wxT("INSERT INTO fts_%s(docid,content)SELECT DISTINCT id,LOW(%s)FROM %s"), name.c_str(), field.c_str(), table.c_str());
	ExecuteUpdate(sql);
}

void FbMainDatabase::CreateFullText(bool force, FbThread * thread)
{
	if ( !force && TableExists(wxT("fts_book_content")) ) return;
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_IMMEDIATE);
	CreateTableFTS(wxT("auth"), wxT("authors"), wxT("full_name"));
	CreateTableFTS(wxT("book"), wxT("books"), wxT("title"));
	CreateTableFTS(wxT("seqn"), wxT("sequences"), wxT("value"));
	ExecuteUpdate(fbT("UPDATE books SET deleted=NULL WHERE deleted=0"));
	ExecuteUpdate(fbT("UPDATE authors SET letter=LTTR(full_name) WHERE id"));
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
	wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_IMMEDIATE);

	/** TABLE params **/
	ExecuteUpdate(fbT("CREATE TABLE config(id INTEGER PRIMARY KEY,value INTEGER,text)"));
	ExecuteUpdate(fbT("INSERT INTO config(id,text)VALUES(1,'MyRuLib local config')"));
	ExecuteUpdate(fbT("INSERT INTO config(id,value)VALUES(2,1)"));

	/** TABLE types **/
	ExecuteUpdate(fbT("CREATE TABLE types(file_type varchar(99) primary key,command,convert)"));

	/** TABLE comments **/
	ExecuteUpdate(fbT("CREATE TABLE comments(id INTEGER PRIMARY KEY,md5sum CHAR(32),rating INTEGER,posted datetime,caption,comment)"));
	ExecuteUpdate(fbT("CREATE INDEX comments_book ON comments(md5sum)"));

	/** TABLE folders **/
	ExecuteUpdate(fbT("CREATE TABLE folders(id INTEGER PRIMARY KEY,value)"));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value)VALUES(-1,'%s')"), _("The best")));
	ExecuteUpdate(wxString::Format(wxT("INSERT INTO folders(id,value)VALUES(-2,'%s')"), _("Other")));

	/** TABLE favorites **/
	ExecuteUpdate(fbT("CREATE TABLE favorites(md5sum CHAR(32),id_folder INTEGER,PRIMARY KEY(md5sum,id_folder))"));
	ExecuteUpdate(fbT("CREATE INDEX favorites_folder ON favorites(id_folder)"));

	trans.Commit();
}

void FbConfigDatabase::DoUpgrade(int version)
{
	switch (version) {
		case 2: {
			/** TABLE states **/
			ExecuteUpdate(fbT("CREATE TABLE states(md5sum CHAR(32) PRIMARY KEY,rating INTEGER,download INTEGER)"));
			ExecuteUpdate(fbT("CREATE INDEX states_rating ON states(rating)"));
		} break;
		case 3: {
			/** TABLE script **/
			ExecuteUpdate(fbT("CREATE TABLE script(id INTEGER PRIMARY KEY,name,text)"));
		} break;
		case 4: {
			/** TABLE script **/
			ExecuteUpdate(fbT("UPDATE states SET download=-2-download WHERE download>0"));
			ExecuteUpdate(fbT("UPDATE states SET download=1 WHERE download=-2"));
			ExecuteUpdate(fbT("UPDATE states SET download=101 WHERE download=-1"));
		} break;
	}
}
