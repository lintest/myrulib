/***************************************************************
 * Name:      DBCreator.cpp
 * Purpose:   Code for Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include "MyrulibData.h"
#include "FbParams.h"
#include <DatabaseLayerException.h>
#include <DatabaseErrorReporter.h>
#include <DatabaseStringConverter.h>
#include <SqlitePreparedStatement.h>
#include "FbConst.h"
#include "MyRuLibApp.h"

bool MyrulibDatabaseLayer::CreateDatabase(const wxString & filename)
{

	wxString msg = _("Database does not exist... recreating:");
	wxMessageBox(msg + wxT("\n") + filename);

    wxLogInfo(wxT("Create database: %s"), filename.c_str());

	bool ok = true;

    BeginTransaction();

    /** TABLE authors **/
	ok &= ExecuteUpdate(wxT("\
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
	ok &= ExecuteUpdate(sql);
    ok &= ExecuteUpdate(wxT("CREATE INDEX author_id ON authors(id);"));
	ok &= ExecuteUpdate(wxT("CREATE INDEX author_letter ON authors(letter);"));
	ok &= ExecuteUpdate(wxT("CREATE INDEX author_name ON authors(search_name);"));

    /** TABLE books **/
	ok &= ExecuteUpdate(wxT("\
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
	ok &= ExecuteUpdate(wxT("CREATE INDEX book_id ON books(id);"));
	ok &= ExecuteUpdate(wxT("CREATE INDEX book_author ON books(id_author);"));
	ok &= ExecuteUpdate(wxT("CREATE INDEX book_archive ON books(id_archive);"));

    /** TABLE archives **/
    ok &= ExecuteUpdate(wxT("\
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
	ok &= ExecuteUpdate(wxT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null);"));
    ok &= ExecuteUpdate(wxT("CREATE INDEX sequences_name ON sequences(value);"));

    /** TABLE bookseq **/
	ok &= ExecuteUpdate(wxT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer);"));
    ok &= ExecuteUpdate(wxT("CREATE INDEX bookseq_book ON bookseq(id_book);"));
    ok &= ExecuteUpdate(wxT("CREATE INDEX bookseq_author ON bookseq(id_author);"));

    /** TABLE words **/
    ok &= ExecuteUpdate(wxT("CREATE TABLE words(word varchar(99), id_book integer not null, number integer);"));
    ok &= ExecuteUpdate(wxT("CREATE INDEX words_word ON words(word);"));

    /** TABLE params **/
    ok &= ExecuteUpdate(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
    ok &= ExecuteUpdate(_("INSERT INTO params(id, text)  VALUES (1, 'Test Library');"));
    ok &= ExecuteUpdate(_("INSERT INTO params(id, value) VALUES (2, 1);"));

    if (ok)
        Commit();
    else
		RollBack();

	return ok;
}

bool MyrulibDatabaseLayer::UpgradeDatabase()
{
	bool ok = true;

	FbParams::LoadParams();
	int version = FbParams::GetValue(DB_LIBRARY_VERSION);

    if (ok && version == 1) {
        wxLogInfo(wxT("Upgrade database to version 2."));
        BeginTransaction();
        ok &= ExecuteUpdate(wxT("ALTER TABLE books ADD sha1sum VARCHAR(27);"));
        ok &= ExecuteUpdate(wxT("CREATE INDEX books_sha1sum ON books(sha1sum);"));
        ok &= ExecuteUpdate(wxT("CREATE INDEX book_filesize ON books(file_size);"));

        ok &= ExecuteUpdate(wxT("CREATE TABLE zip_books(book varchar(99), file integer);"));
        ok &= ExecuteUpdate(wxT("CREATE TABLE zip_files(file integer primary key, path text);"));
        ok &= ExecuteUpdate(wxT("CREATE INDEX zip_books_name ON zip_books(book);"));

        version ++;
        FbParams().SetValue(DB_LIBRARY_VERSION, version);
        if (ok)
            Commit();
        else
            RollBack();
    }

    if (ok && version == 2) {
        wxLogInfo(wxT("Upgrade database to version 3."));
        BeginTransaction();
        ok &= ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99), command text, convert text);"));
        ok &= ExecuteUpdate(wxT("CREATE UNIQUE INDEX types_file_type ON types(file_type);"));
        ok &= ExecuteUpdate(wxT("DROP INDEX IF EXISTS book_file;"));

        ok &= ExecuteUpdate(wxT("CREATE TABLE files(id_book integer, id_archive integer, file_name text);"));
        ok &= ExecuteUpdate(wxT("CREATE INDEX files_book ON files(id_book);"));

        version ++;
        FbParams().SetValue(DB_LIBRARY_VERSION, version);
        if (ok)
            Commit();
        else
            RollBack();
    }

	FbParams::LoadParams();
	version = FbParams::GetValue(DB_LIBRARY_VERSION);

	if (version != 3) {
		wxLogFatalError(_("Mismatched database versions."));
		return false;
	}

	return true;
}

static void SQLiteLowerCase(sqlite3_context *p, int nArg, sqlite3_value **apArg)
{
	const char * zInput = (const char *) sqlite3_value_text(apArg[0]);

	DatabaseStringConverter conv;
	wxString text = conv.ConvertFromUnicodeStream( zInput );

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

    MyrulibDatabaseLayer::sm_Current++;
    if (MyrulibDatabaseLayer::sm_Current >= MyrulibDatabaseLayer::sm_Delta) {
        MyrulibDatabaseLayer::sm_Current = 0;
        MyrulibDatabaseLayer::sm_Progress++;
        wxUpdateUIEvent event( ID_PROGRESS_UPDATE );
        event.SetText(MyrulibDatabaseLayer::sm_msg);
        event.SetInt(MyrulibDatabaseLayer::sm_Progress);
        wxPostEvent(wxGetApp().GetTopWindow(), event);
    }

	wxCharBuffer buffer = conv.ConvertToUnicodeStream(text);
	sqlite3_result_text(p, buffer, -1, SQLITE_TRANSIENT);
}

bool MyrulibDatabaseLayer::Open(const wxString& strDatabase)
{
	bool result = SqliteDatabaseLayer::Open(strDatabase);
	sqlite3_create_function(GetSQLite(), "LOWER", 1, SQLITE_ANY, NULL, SQLiteLowerCase, NULL, NULL);
	return result;
}

int MyrulibDatabaseLayer::sm_Progress = 0;
int MyrulibDatabaseLayer::sm_Current = 0;
int MyrulibDatabaseLayer::sm_Delta = 0;
wxString MyrulibDatabaseLayer::sm_msg;



