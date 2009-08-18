/***************************************************************
 * Name:      DBCreator.cpp
 * Purpose:   Code for Application Class
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-22
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include "DBCreator.h"
#include "FbParams.h"
#include <DatabaseLayerException.h>

DBCreator::DBCreator(DatabaseLayer * database) {
	m_Database = database;
}

DBCreator::~DBCreator(void) {
}

extern wxString strNobody;

bool DBCreator::CreateDatabase(void)
{

	wxMessageBox(_("Database does not exist... recreating."));

	// Try to recreate tables
	try {
		m_Database->RunQuery(wxT("\
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
		m_Database->RunQuery(sql);
		m_Database->RunQuery(wxT("CREATE INDEX author_id ON authors(id);"));
		m_Database->RunQuery(wxT("CREATE INDEX author_letter ON authors(letter);"));
		m_Database->RunQuery(wxT("CREATE INDEX author_name ON authors(search_name);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		m_Database->RunQuery(wxT("\
            CREATE TABLE books(\
                id integer not null,\
                id_author integer not null,\
                title varchar(255) not null,\
                annotation text,\
                genres text,\
                deleted boolean,\
                id_archive integer,\
                file_name varchar(255),\
                file_size integer,\
                file_type varchar(20),\
                description text);\
        "));
		m_Database->RunQuery(wxT("CREATE INDEX book_id ON books(id);"));
		m_Database->RunQuery(wxT("CREATE INDEX book_author ON books(id_author);"));
		m_Database->RunQuery(wxT("CREATE INDEX book_archive ON books(id_archive);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		m_Database->RunQuery(wxT("\
            CREATE TABLE archives(\
                id integer primary key,\
                file_name varchar(255),\
                file_path varchar(255),\
                file_size integer,\
                file_count integer,\
                min_id_book integer,\
                max_id_book integer,\
                file_type varchar(20),\
                description text);\
        "));
		m_Database->RunQuery(wxT("CREATE INDEX book_file ON archives(file_name);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
        m_Database->RunQuery(wxT("CREATE TABLE params(id integer primary key, value integer, text text);"));
        m_Database->RunQuery(_("INSERT INTO params(id, text)  VALUES (1, 'Test Library');"));
        m_Database->RunQuery(_("INSERT INTO params(id, value) VALUES (2, 1);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		m_Database->RunQuery(wxT("CREATE TABLE sequences(id integer primary key, value varchar(255) not null);"));
		m_Database->RunQuery(wxT("CREATE INDEX sequences_name ON sequences(value);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		m_Database->RunQuery(wxT("CREATE TABLE bookseq(id_book integer, id_seq integer, number integer, level integer, id_author integer);"));
		m_Database->RunQuery(wxT("CREATE INDEX bookseq_book ON sequences(id_book);"));
		m_Database->RunQuery(wxT("CREATE INDEX bookseq_author ON sequences(id_author);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		m_Database->RunQuery(wxT("CREATE TABLE words(word varchar(99), id_book integer not null, number integer);"));
		m_Database->RunQuery(wxT("CREATE INDEX words_word ON words(word);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	return true;
}

void DBCreator::UpgradeDatabase()
{
    int version = FbParams::GetValue(DB_LIBRARY_VERSION);

    if (version == 1) {
        try {
            m_Database->BeginTransaction();
            m_Database->RunQuery(wxT("ALTER TABLE books ADD sha1sum VARCHAR(27);"));
            m_Database->RunQuery(wxT("CREATE INDEX books_sha1sum ON books(sha1sum);"));
            m_Database->RunQuery(wxT("CREATE INDEX book_filesize ON books(file_size);"));
            version ++;
            FbParams().SetValue(DB_LIBRARY_VERSION, version);
            m_Database->Commit();
        }
        catch(DatabaseLayerException & e) {
            m_Database->RollBack();
            wxUnusedVar(e);
        }
    }
}
