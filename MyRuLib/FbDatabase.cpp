#include "FbDatabase.h"
#include "FbParams.h"
#include "FbConst.h"
#include "MyRuLibApp.h"

wxCriticalSection FbDatabase::sm_queue;

void FbMainDatabase::CreateDatabase()
{
    wxSQLite3Transaction trans(this);

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
    ExecuteUpdate(_("INSERT INTO params(id, text)  VALUES (1, 'Test Library')"));
    ExecuteUpdate(_("INSERT INTO params(id, value) VALUES (2, 1)"));

    trans.Commit();
}

void FbMainDatabase::UpgradeDatabase()
{
	FbParams params(this);
	params.LoadParams();
	int version = FbParams::GetValue(DB_LIBRARY_VERSION);

	wxString sUpgradeMsg = wxT("Upgrade database to version %d");

    if (version == 1) {
        version ++;
        wxLogInfo(sUpgradeMsg, version);
        wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

        /** TABLE books **/
        ExecuteUpdate(wxT("ALTER TABLE books ADD sha1sum VARCHAR(27)"));
        ExecuteUpdate(wxT("CREATE INDEX books_sha1sum ON books(sha1sum)"));
        ExecuteUpdate(wxT("CREATE INDEX book_filesize ON books(file_size)"));

        /** TABLE zip_books, zip_files **/
        ExecuteUpdate(wxT("CREATE TABLE zip_books(book varchar(99), file integer)"));
        ExecuteUpdate(wxT("CREATE TABLE zip_files(file integer primary key, path text)"));
        ExecuteUpdate(wxT("CREATE INDEX zip_books_name ON zip_books(book)"));

        params.SetValue(DB_LIBRARY_VERSION, version);
        trans.Commit();
    }

    if (version == 2) {
        version ++;
        wxLogInfo(sUpgradeMsg, version);
        wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

        /** TABLE types **/
        ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99), command text, convert text)"));
        ExecuteUpdate(wxT("CREATE UNIQUE INDEX types_file_type ON types(file_type)"));
        ExecuteUpdate(wxT("DROP INDEX IF EXISTS book_file"));

        /** TABLE files **/
        ExecuteUpdate(wxT("CREATE TABLE files(id_book integer, id_archive integer, file_name text)"));
        ExecuteUpdate(wxT("CREATE INDEX files_book ON files(id_book)"));

        params.SetValue(DB_LIBRARY_VERSION, version);
        trans.Commit();
    }

    if (version == 3) {
        version ++;
        wxLogInfo(sUpgradeMsg, version);
        wxSQLite3Transaction trans(this, WXSQLITE_TRANSACTION_EXCLUSIVE);

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

        params.SetValue(DB_LIBRARY_VERSION, version);
        trans.Commit();
    }

	params.LoadParams();
	int old_version = params.GetValue(DB_LIBRARY_VERSION);

    int new_version = 4;
	if (old_version != new_version) {
		wxLogFatalError(_("Database version mismatch. Need a new version %d, but used the old %d."), new_version, old_version);
	}
}

void FbLowerFunction::Execute(wxSQLite3FunctionContext& ctx)
{
    int argCount = ctx.GetArgCount();
    if (argCount != 1) {
        ctx.SetResultError(wxString::Format(_("LOWER called with wrong number of arguments: %d."), argCount));
        return;
    }
    wxString text = ctx.GetString(0);
#if defined(__WIN32__)
    int len = text.length() + 1;
    wxChar * buf = new wxChar[len];
    wxStrcpy(buf, text.c_str());
    CharLower(buf);
    text = buf;
    delete [] buf;
#else
    text.MakeLower();
#endif
    ctx.SetResult(text);
}

void FbGenreFunction::Execute(wxSQLite3FunctionContext& ctx)
{
    int argCount = ctx.GetArgCount();
    if (argCount != 2) {
        ctx.SetResultError(wxString::Format(_("GENRE called with wrong number of arguments: %d."), argCount));
        return;
    }
    wxString text = ctx.GetString(0);
    wxString genre = ctx.GetString(1);

    for (size_t i=0; i<text.Length()/2; i++) {
    	if ( text.Mid(i*2, 2) == genre ) {
    		ctx.SetResult(true);
    		return;
    	}
    }
    ctx.SetResult(false);
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

int FbDatabase::NewId(int iParam)
{
    wxCriticalSectionLocker enter(sm_queue);

    int iValue = 0;
    {
        wxString sql = wxT("SELECT value FROM params WHERE id=?");
        wxSQLite3Statement stmt = PrepareStatement(sql);
        stmt.Bind(1, iParam);
        wxSQLite3ResultSet result = stmt.ExecuteQuery();
        if (result.NextRow()) iValue = result.GetInt(0);
    }

    iValue++;

    {
        wxString sql = wxT("INSERT OR REPLACE INTO params(value, id) VALUES(?,?)");
        wxSQLite3Statement stmt = PrepareStatement(sql);
        stmt.Bind(1, iValue);
        stmt.Bind(2, iParam);
        stmt.ExecuteUpdate();
    }
	return iValue;
}

FbCommonDatabase::FbCommonDatabase()
    :FbDatabase()
{
    FbDatabase::Open(wxGetApp().GetAppData());
}

void FbMainDatabase::Open(const wxString& fileName, const wxString& key, int flags)
{
    bool bExists = wxFileExists(fileName);

    if (bExists)
        wxLogInfo(wxT("Open database: %s"), fileName.c_str());
    else {
        wxLogInfo(wxT("Create database: %s"), fileName.c_str());
    	wxString msg = _("Database does not exist... recreating:");
    	wxMessageBox(msg + wxT("\n") + fileName);
    }

    try {
        FbDatabase::Open(fileName, key, flags);
        if (!bExists) CreateDatabase();
        UpgradeDatabase();
    }
    catch (wxSQLite3Exception & e) {
        wxLogFatalError(e.GetMessage());
    }
}

