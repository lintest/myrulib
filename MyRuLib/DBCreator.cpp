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

bool DBCreator::CreateDatabase(void){

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
				id_sequence integer,\
                deleted boolean,\
                id_archive integer,\
                file_name varchar(255),\
                file_size integer,\
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
                min_number integer,\
                max_number integer,\
                description text);\
        "));
		m_Database->RunQuery(wxT("CREATE INDEX book_file ON archives(file_name);"));
	}
	catch(DatabaseLayerException & e) {wxUnusedVar(e);}

	try {
		FbParams::InitParams(m_Database);
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

	return true;
}
