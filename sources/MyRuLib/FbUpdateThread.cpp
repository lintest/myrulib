#include "FbUpdateThread.h"
#include "FbDatabase.h"
#include "FbDateTime.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbCounter.h"
#include "MyRuLibApp.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

//-----------------------------------------------------------------------------
//  FbUpdateItem
//-----------------------------------------------------------------------------

FbUpdateThread::FbUpdateThread()
{
	wxURL(strHomePage).GetProtocol().SetTimeout(10);
}

void * FbUpdateThread::Entry()
{
	int date = FbParams::GetInt(DB_DATAFILE_DATE);
	int today = FbDateTime::Today().Code() + 20000000;
	wxString type = Lower(FbParams::GetStr(DB_LIBRARY_TYPE));
	if (date == 0) return NULL;

	FbCommonDatabase database;
	FbCounter counter(database);

	bool ok = false;
	while (date && date < today) {
		FbUpdateItem item(database, date, type);
		date = item.Execute();
		if (date) {
			FbParams::Set(DB_DATAFILE_DATE, date);
			ok = true;
		}
	}

	if (ok) {
		counter.Execute();
		wxLogWarning(_("Database successfully updated"));
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//  FbUpdateItem
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbUpdateItem, wxObject)

wxString FbUpdateItem::GetAddr(int date, const wxString &type)
{
	wxString addr = strHomePage;
	addr << wxT('/') << type << wxT('/') << date << wxT(".zip");
	return addr;
}

FbUpdateItem::FbUpdateItem(wxSQLite3Database & database, int code, const wxString &type)
	: m_database(database), m_code(code), m_url(GetAddr(code, type))
{
}

FbUpdateItem::~FbUpdateItem()
{
	if (!m_filename.IsEmpty()) wxRemoveFile(m_filename);
	if (!m_dataname.IsEmpty()) wxRemoveFile(m_dataname);
}

int FbUpdateItem::Execute()
{
	bool ok = OpenURL();
	if (ok) ok = ReadURL();
	if (ok) ok = OpenZip();
	if (ok) return DoUpdate();
	return 0;
}

bool FbUpdateItem::OpenURL()
{
	m_input = m_url.GetInputStream();

	wxHTTP & http = (wxHTTP&)m_url.GetProtocol();
	if (http.GetResponse() == 404) {
		FbLogError(_("The update file is missing"), m_url.GetURL());
		return false;
	}

	FbLogWarning(_("Update collection"), m_url.GetURL());

	if (m_url.GetError() != wxURL_NOERR) {
		FbLogError(_("Download error"), m_url.GetURL());
		return false;
	}

	return true;
}

bool FbUpdateItem::ReadURL()
{
	m_filename = wxFileName::CreateTempFileName(wxT("~"));
	wxFileOutputStream out(m_filename);

	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];
	size_t size = m_input->GetSize();
	if (!size) size = 0xFFFFFFFF;
	size_t count = 0;
	size_t pos = 0;

	int step = 1;
	do {
		FbProgressEvent(ID_PROGRESS_UPDATE, m_url.GetURL(), pos/(size/1000), _("File download")).Post();
		count = m_input->Read(buf, BUFSIZE).LastRead();
		if ( count ) {
			out.Write(buf, count);
			pos += count;
		} else step++;
	} while (pos < size && step < 5);

	FbProgressEvent(ID_PROGRESS_UPDATE).Post();

	return true;
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
	}
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

	wxSQLite3Transaction trans(&m_database, WXSQLITE_TRANSACTION_EXCLUSIVE);

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
		{
			wxT("tmp_a"), wxT("id"), 
			wxT("books"), wxT("id"),
		},
		{
			wxT("tmp_s"), wxT("id"), 
			wxT("sequences"), wxT("id"),
		},
		{	
			wxT("tmp_d"), wxT("id"), 
			wxT("books"), wxT("created"),
		},
	};

	size_t size = sizeof( list ) / sizeof( wxChar * ) / 4;
	for (size_t i = 0; i < size; i++) {
		wxString sql = wxString::Format(wxT("INSERT OR REPLACE INTO %s(%s)SELECT DISTINCT %s FROM upd.%s"), list[i][0], list[i][1], list[i][3], list[i][2]);
		int count = m_database.ExecuteUpdate(sql);
		if (i == 0) wxLogWarning(_("Loaded new %d books"), count);
	}

	trans.Commit();

	return date;
}
