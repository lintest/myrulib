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

extern wxString strNobody;

bool MyrulibDatabaseLayer::CreateDatabase(const wxString & filename)
{

	wxString msg = _("Database does not exist... recreating:");
	wxMessageBox(msg + wxT("\n") + filename);

	// Try to recreate tables
	try {
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
		ExecuteUpdate(wxT("CREATE INDEX author_id ON authors(id);"));
		ExecuteUpdate(wxT("CREATE INDEX author_letter ON authors(letter);"));
		ExecuteUpdate(wxT("CREATE INDEX author_name ON authors(search_name);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
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
		ExecuteUpdate(wxT("CREATE INDEX book_id ON books(id);"));
		ExecuteUpdate(wxT("CREATE INDEX book_author ON books(id_author);"));
		ExecuteUpdate(wxT("CREATE INDEX book_archive ON books(id_archive);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
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
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
        ExecuteUpdate(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
        ExecuteUpdate(_("INSERT INTO params(id, text)  VALUES (1, 'Test Library');"));
        ExecuteUpdate(_("INSERT INTO params(id, value) VALUES (2, 1);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		ExecuteUpdate(wxT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null);"));
		ExecuteUpdate(wxT("CREATE INDEX sequences_name ON sequences(value);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		ExecuteUpdate(wxT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer);"));
		ExecuteUpdate(wxT("CREATE INDEX bookseq_book ON sequences(id_book);"));
		ExecuteUpdate(wxT("CREATE INDEX bookseq_author ON sequences(id_author);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		ExecuteUpdate(wxT("CREATE TABLE words(word varchar(99), id_book integer not null, number integer);"));
		ExecuteUpdate(wxT("CREATE INDEX words_word ON words(word);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	return true;
}

bool MyrulibDatabaseLayer::UpgradeDatabase()
{
	try {
		FbParams::LoadParams();
		int version = FbParams::GetValue(DB_LIBRARY_VERSION);

		if (version == 1) {
            BeginTransaction();
            ExecuteUpdate(wxT("ALTER TABLE books ADD sha1sum VARCHAR(27);"));
            ExecuteUpdate(wxT("CREATE INDEX books_sha1sum ON books(sha1sum);"));
            ExecuteUpdate(wxT("CREATE INDEX book_filesize ON books(file_size);"));

			ExecuteUpdate(wxT("CREATE TABLE zip_books(book varchar(99), file integer);"));
			ExecuteUpdate(wxT("CREATE TABLE zip_files(file integer primary key, path text);"));
			ExecuteUpdate(wxT("CREATE INDEX zip_books_name ON zip_books(book);"));

            version ++;
            FbParams().SetValue(DB_LIBRARY_VERSION, version);
            Commit();
        }

		if (version == 2) {
            BeginTransaction();
			ExecuteUpdate(wxT("CREATE TABLE types(file_type varchar(99), command text, convert text);"));
			ExecuteUpdate(wxT("CREATE UNIQUE INDEX types_file_type ON types(file_type);"));
			ExecuteUpdate(wxT("DROP INDEX IF EXISTS book_file;"));

			ExecuteUpdate(wxT("CREATE TABLE files(id_book integer, id_archive integer, file_name text);"));
			ExecuteUpdate(wxT("CREATE INDEX files_book ON files(id_book);"));

            version ++;
            FbParams().SetValue(DB_LIBRARY_VERSION, version);
            Commit();
        }
    } catch(DatabaseLayerException & e) {
		RollBack();
		throw e;
	}

	FbParams::LoadParams();
	int version = FbParams::GetValue(DB_LIBRARY_VERSION);

	if (version != 3) {
		throw DatabaseLayerException(DATABASE_LAYER_ERROR, wxT("Mismatched database versions."));
	}

	return true;
}

#if defined(__WIN32__)
static void SQLiteLowerCase(sqlite3_context *p, int nArg, sqlite3_value **apArg)
{
	const char * zInput = (const char *) sqlite3_value_text(apArg[0]);

	DatabaseStringConverter conv;
	wxString text = conv.ConvertFromUnicodeStream( zInput );

	int len = text.length() + 1;
	wxChar * buf = new wxChar[len];
	wxStrcpy(buf, text.c_str());
	CharLower(buf);
	text = buf;
	delete [] buf;

	wxCharBuffer buffer = conv.ConvertToUnicodeStream(text);
	sqlite3_result_text(p, buffer, -1, SQLITE_TRANSIENT);
}
#endif

bool MyrulibDatabaseLayer::Open(const wxString& strDatabase)
{
	bool result = SqliteDatabaseLayer::Open(strDatabase);
#if defined(__WIN32__)
	sqlite3_create_function(GetSQLite(), "LOWER", 1, SQLITE_ANY, NULL, SQLiteLowerCase, NULL, NULL);
#endif
	return result;
}
