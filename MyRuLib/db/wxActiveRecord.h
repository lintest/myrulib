///////////////////////////////////////////////////////////////////////////////
// Name:        wxActiveRecord.h
// Purpose:     Base classes of wxActiveRecord
// Author:      Matías Szeftel
// Modified by:
// Created:     01/10/2006
// Copyright:   (c) 2006 Matías Szeftel - mszeftel@yahoo.com.ar
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_ACTIVE_RECORD_H_
#define _WX_ACTIVE_RECORD_H_

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/wfstream.h>
#include <wx/tokenzr.h>
#include <DatabaseLayer.h>
#include <DatabaseErrorCodes.h>
#include <DatabaseLayerException.h>

//WX_DEFINE_ARRAY DOESN'T WORK WITH FUNCTION POINTERS
#include <vector>

// COMMENT OUT THE ONES YOU DON'T USE

#define AR_USE_SQLITE
//#define AR_USE_POSTGRESQL
//#define AR_USE_MYSQL
//#define AR_USE_FIREBIRD

//*************************************//


class wxActiveRecord;
class wxActiveRecordRow;
class wxActiveRecordRowSet;


WX_DEFINE_ARRAY(wxActiveRecordRow* , ArrayOfRows);
WX_DEFINE_ARRAY(wxActiveRecordRowSet* , ArrayOfRowSets);

//begin WX_ACTIVE_RECORD

class wxActiveRecord{
protected:
        DatabaseLayer* m_database;
        wxString m_table;
        bool m_generatedConnection;
        ArrayOfRows garbageRows;
        ArrayOfRowSets garbageRowSets;

public:
        wxActiveRecord();
        /**
        \param table Is the table or view with you want to access.
        \param name Is the filename for sqlite3 or the database name for postgres and mysql
        */
        wxActiveRecord(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        /**
        \param table Is the table or view with you want to access.
        \param database is a DatabaseLyaer pointer so you can reuse an open connection
        */
        wxActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);

        virtual ~wxActiveRecord();
        /**
        \param table Is the table or view with you want to access.
        \param name Is the filename for sqlite3 or the database name for postgres and mysql

        */
        virtual bool Create(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString)=0;
        /**
        \param table Is the table or view with you want to access.
        \param database is a DatabaseLyaer pointer so you can reuse an open connection
        */
        virtual bool Create(DatabaseLayer* database,const wxString& table=wxEmptyString);

        /**
         Returns the database.
        */
        virtual DatabaseLayer* GetDatabase() const;
        /**
        Returns the name of the table.
        */
        virtual wxString GetTable() const;

        /**
        Shows a dialog message with exception details.
        */
        static void ProcessException(DatabaseLayerException& e);

        void CollectRow(wxActiveRecordRow* row);
        void CollectRowSet(wxActiveRecordRowSet* set);

};

//end WX_ACTIVE_RECORD


//begin WX_ACTIVE_RECORD_ROW

class wxActiveRecordRow{
protected:
        DatabaseLayer* m_database;
        wxString m_table;
        bool newRow;
        ArrayOfRows garbageRows;
        ArrayOfRowSets garbageRowSets;
public:
        wxActiveRecordRow();
        wxActiveRecordRow(wxActiveRecord* activeRecord);
        wxActiveRecordRow(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual ~wxActiveRecordRow();
        virtual bool Create(wxActiveRecord* activeRecord);
        virtual bool Create(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual bool Save();
        virtual bool Delete();
        void CollectRow(wxActiveRecordRow* row);
        void CollectRowSet(wxActiveRecordRowSet* set);
public:
        long id;
};

//end WX_ACTIVE_RECORD_ROW

//begin WX_ACTIVE_RECORD_ROW_SET

typedef int (*CMPFUNC_proto)(wxActiveRecordRow**,wxActiveRecordRow**);
WX_DEFINE_ARRAY_SHORT(bool , AscArr);

//WX_DEFINE_ARRAY(CMPFUNC_proto, CmpArr); deprecated?


class wxActiveRecordRowSet : public ArrayOfRows{
protected:
        DatabaseLayer* m_database;
        wxString m_table;
        //static CmpArr cmpArr; deprecated?
		static std::vector<CMPFUNC_proto> cmpArr;

        static AscArr ascArr;
        static wxString lastSort;
        static int CMPFUNC_default(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
        static int CMPFUNC_global(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
        virtual CMPFUNC_proto GetCmpFunc(const wxString& var) const;

public:
        wxActiveRecordRowSet();
        wxActiveRecordRowSet(wxActiveRecord* activeRecord);
        wxActiveRecordRowSet(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual ~wxActiveRecordRowSet();
        virtual bool Create(wxActiveRecord* activeRecord);
        virtual bool Create(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual bool SaveAll();
        virtual void SortBy(const wxString& sortBy);
        virtual void ReSort();
};

//end WX_ACTIVE_RECORD_ROW_SET


//begin WX_SQLITE_ACTIVE_RECORD
#ifdef AR_USE_SQLITE
#include <SqliteDatabaseLayer.h>
class wxSqliteActiveRecord : public wxActiveRecord{
protected:
        wxSqliteActiveRecord();
        wxSqliteActiveRecord(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        wxSqliteActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual bool Create(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
};
#endif //AR_USE_SQLITE
//end WX_SQLITE_ACTIVE_RECORD

//begin WX_POSTGRESQL_ACTIVE_RECORD
#ifdef AR_USE_POSTGRESQL

#include <PostgresDatabaseLayer.h>

class wxPostgresActiveRecord : public wxActiveRecord{
protected:
        wxPostgresActiveRecord();
        wxPostgresActiveRecord(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        wxPostgresActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual bool Create(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
};

#endif //AR_USE_POSTGRESQL
//end WX_POSTGRESQL_ACTIVE_RECORD

//begin WX_MYSQL_ACTIVE_RECORD
#ifdef AR_USE_MYSQL

#include <MysqlDatabaseLayer.h>

class wxMysqlActiveRecord : public wxActiveRecord{
protected:
        wxMysqlActiveRecord();
        wxMysqlActiveRecord(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        wxMysqlActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);
        virtual bool Create(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
};


#endif //AR_USE_MYSQL

//end WX_MYSQL_ACTIVE_RECORD

//begin WX_FIREBIRD_ACTIVE_RECORD
#ifdef AR_USE_FIREBIRD

#include <FirebirdDatabaseLayer.h>

class wxFirebirdActiveRecord : public wxActiveRecord{
protected:
        wxFirebirdActiveRecord();
        wxFirebirdActiveRecord(const wxString& name,const wxString& server,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        wxFirebirdActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);

        virtual bool Create(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
};

// when using his class as base class of an AR the server string will be ignored.
// And name shall be the file name.
class wxFirebirdEmbeddedActiveRecord : public wxActiveRecord{
protected:
        wxFirebirdEmbeddedActiveRecord();
        wxFirebirdEmbeddedActiveRecord(const wxString& name,const wxString& server,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
        wxFirebirdEmbeddedActiveRecord(DatabaseLayer* database,const wxString& table=wxEmptyString);

        virtual bool Create(const wxString& name,const wxString& server=wxT("localhost"),const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxEmptyString);
};

#endif //AR_USE_FIREBIRD

//end WX_FIREBIRD_ACTIVE_RECORD

#endif /* _WX_ACTIVE_RECORD_H_ */
