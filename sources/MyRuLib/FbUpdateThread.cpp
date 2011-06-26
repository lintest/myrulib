#include "FbUpdateThread.h"
#include "FbInternetBook.h"
#include "FbDatabase.h"
#include "FbDateTime.h"
#include "FbConst.h"
#include "FbParams.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

//-----------------------------------------------------------------------------
//  FbUpdateItem
//-----------------------------------------------------------------------------

FbUpdateThread::FbUpdateThread()
{
	wxURL(MyRuLib::HomePage()).GetProtocol();
}

void * FbUpdateThread::Entry()
{
	int date = FbParams(DB_DATAFILE_DATE);
	int today = FbDateTime::Today().Code() + 20000000;
	wxString type = Lower(FbParams(DB_LIBRARY_TYPE));
	if (date == 0) return NULL;

	FbCommonDatabase database;

	bool ok = false;
	while (date && date < today) {
		FbUpdateItem item(database, date, type);
		date = item.Execute();
		if (date) {
			FbParams(DB_DATAFILE_DATE) = date;
			ok = true;
		}
	}

	if (ok) wxLogWarning(_("Database successfully updated"));

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbUpdateItem
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbUpdateItem, wxObject)

wxString FbUpdateItem::GetAddr(int date, const wxString &type)
{
	return wxString::Format(wxT("%s/%s/%d/%d.zip"), MyRuLib::HomePage().c_str(), type.c_str(), date / 10000, date);
}

FbUpdateItem::FbUpdateItem(wxSQLite3Database & database, int code, const wxString &type)
	: m_database(database), m_code(code), m_type(type), m_url(GetAddr(code, type))
{
}

FbUpdateItem::~FbUpdateItem()
{
	if (!m_filename) wxRemoveFile(m_filename);
	if (!m_dataname) wxRemoveFile(m_dataname);
}

int FbUpdateItem::Execute()
{
	FbLogWarning(_("Update collection"), m_url);
	bool ok = FbInternetBook::Download(m_url, m_filename);
	if (ok) ok = OpenZip();
	if (ok) return DoUpdate();
	return 0;
}

bool FbUpdateItem::OpenZip()
{
	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	bool ok = zip.IsOk();
	if (!ok) return false;

	if (wxZipEntry * entry = zip.GetNextEntry()) {
		ok = zip.OpenEntry(*entry);
		delete entry;
	} else ok = false;

	if (!ok) return false;

	m_dataname = wxFileName::CreateTempFileName(wxT("~"));
	wxFileOutputStream out(m_dataname);
	out.Write(zip);
	return out.IsOk();
}

int FbUpdateItem::DoUpdate()
{
	{
		wxString sql = wxT("ATTACH ? AS upd");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, m_dataname);
		stmt.ExecuteUpdate();
	}

	int date = 0;
	{
		wxString sql = wxT("SELECT value FROM upd.params WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, DB_DATAFILE_DATE);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) date = result.GetInt(0);
	}
	if (date <= m_code) return 0;

	{
		wxString sql = wxT("SELECT text FROM upd.params WHERE id=?");
		wxSQLite3Statement stmt = m_database.PrepareStatement(sql);
		stmt.Bind(1, DB_LIBRARY_TYPE);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if (!(result.NextRow() && result.GetString(0).Lower() == m_type)) {
			FbLogError(_("Wrong update library type"), result.GetString(0));
			return 0;
		}
	}

	wxSQLite3Transaction trans(&m_database, WXSQLITE_TRANSACTION_EXCLUSIVE);

	ExecDelete();
	ExecInsert();
	CalcCount();

	trans.Commit();

	m_database.ExecuteUpdate(wxT("DETACH upd"));

	return date;
}

void FbUpdateItem::CalcCount()
{
	wxString sql = wxT("SELECT COUNT(DISTINCT id) FROM upd.books");
	wxSQLite3ResultSet result = m_database.ExecuteQuery(sql);
	if (result.NextRow()) wxLogWarning(_("Loaded new %d books"), result.GetInt(0));
}

void FbUpdateItem::ExecDelete()
{
	const wxChar * list[][4] = {
		{
			wxT("books"), wxT("id"),
			wxT("books"), wxT("id"),
		},
		{
			wxT("bookseq"), wxT("id_book"),
			wxT("books"), wxT("id"),
		},
		{
			wxT("genres"), wxT("id_book"),
			wxT("books"), wxT("id"),
		},
		{
			wxT("files"), wxT("id_book"),
			wxT("books"), wxT("id"),
		},
		{
			wxT("fts_auth"), wxT("docid"),
			wxT("authors"), wxT("id"),
		},
		{
			wxT("fts_book"), wxT("docid"),
			wxT("books"),     wxT("id"),
		},
		{
			wxT("fts_seqn"),  wxT("docid"),
			wxT("sequences"), wxT("id"),
		},
	};

	size_t size = sizeof( list ) / sizeof( wxChar * ) / 4;
	for (size_t i = 0; i < size; i++) {
		wxString sql = wxString::Format(wxT("DELETE FROM %s WHERE %s IN (SELECT %s FROM upd.%s)"), list[i][0], list[i][1], list[i][3], list[i][2]);
		m_database.ExecuteUpdate(sql);
	}
}

void FbUpdateItem::ExecInsert()
{
	FbLowerFunction	lower;
	FbAuthorFunction author;
	FbLetterFunction letter;

	m_database.CreateFunction(wxT("LOW"), 1, lower);
	m_database.CreateFunction(wxT("AUTH"), 3, author);
	m_database.CreateFunction(wxT("LTTR"), 1, letter);

	const wxChar * list[][4] = {
		{
			wxT("books"), wxT("id,id_author,title,file_name,file_size,file_type,md5sum,genres,lang,created,year,annotation,description"),
			wxT("books"), wxT("id,id_author,title,file_name,file_size,file_type,md5sum,genres,lang,created,year,annotation,description"),
		},
		{
			wxT("authors"), wxT("id,last_name,first_name,middle_name,full_name,search_name,letter"),
			wxT("authors"), wxT("id,last_name,first_name,middle_name,AUTH(last_name,first_name,middle_name),LOW(AUTH(last_name,first_name,middle_name)),LTTR(AUTH(last_name,first_name,middle_name))"),
		},
		{
			wxT("sequences"), wxT("id,value"),
			wxT("sequences"), wxT("id,value"),
		},
		{
			wxT("bookseq"), wxT("id_book,id_seq,number"),
			wxT("bookseq"), wxT("id_book,id_seq,number"),
		},
		{
			wxT("genres"), wxT("id_book,id_genre"),
			wxT("genres"), wxT("id_book,id_genre"),
		},
		{
			wxT("archives"), wxT("id,file_name"),
			wxT("archives"), wxT("id,file_name"),
		},
		{
			wxT("files"), wxT("id_archive,id_book,file_name"),
			wxT("files"), wxT("id_archive,id_book,file_name"),
		},
		{
			wxT("fts_auth"), wxT("docid,content"),
			wxT("authors"), wxT("id,LOW(AUTH(last_name,first_name,middle_name))"),
		},
		{
			wxT("fts_book"), wxT("docid,content"),
			wxT("books"),     wxT("id,LOW(title)"),
		},
		{
			wxT("fts_seqn"),  wxT("docid,content"),
			wxT("sequences"), wxT("id,LOW(value)"),
			},
	};

	size_t size = sizeof( list ) / sizeof( wxChar * ) / 4;
	for (size_t i = 0; i < size; i++) {
		wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(%s)SELECT DISTINCT %s FROM upd.%s"), list[i][0], list[i][1], list[i][3], list[i][2]);
		m_database.ExecuteUpdate(sql);
	}
}

