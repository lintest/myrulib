#include "FbDownloader.h"
#include "MyRuLibApp.h"
#include "FbBookEvent.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbDatabase.h"
#include "BaseThread.h"
#include "polarssl/md5.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

class FbInternetBook
{
	public:
		FbInternetBook();
		bool Execute();
	private:
		bool DoDownload();
		bool CheckMD5();
		bool SaveFile();
	private:
		int m_id;
		wxString m_url;
		wxString m_md5sum;
		wxString m_filetype;
		wxString m_filename;
};

class FbURL: public wxURL
{
	public:
		FbURL(const wxString& sUrl = wxEmptyString);
};

FbURL::FbURL(const wxString& sUrl): wxURL(sUrl)
{
	if (FbParams::GetValue(FB_USE_PROXY))
		SetProxy(FbParams::GetText(FB_PROXY_ADDR));
	GetProtocol().SetTimeout(10);
}

FbInternetBook::FbInternetBook()
	: m_id(0)
{
	wxString sql = wxT("SELECT id, books.md5sum, file_type FROM books JOIN states ON books.md5sum=states.md5sum WHERE download>0 ORDER BY download LIMIT 1");
	try {
		FbCommonDatabase database;
		database.AttachConfig();
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		if ( result.NextRow() ) {
			m_id = result.GetInt(0);
			m_md5sum = result.GetString(1);
			m_filetype = result.GetString(2);
			m_url = FbParams::GetText(FB_LIBRUSEC_URL) + wxString::Format(wxT("/b/%d/download"), m_id);
			m_url = wxT("http://lib.ololo.cc") + wxString::Format(wxT("/b/%d/download/"), m_id);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

bool FbInternetBook::Execute()
{
	return m_id && DoDownload() && CheckMD5() && SaveFile();
}

bool FbInternetBook::DoDownload()
{
	FbURL url(m_url);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(wxT("URL error: ") + m_url);
		return false;
	}
	wxHTTP & http = (wxHTTP&)url.GetProtocol();

	wxInputStream * in = url.GetInputStream();
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(wxT("Connect error: ") + m_url);
		return false;
	}
	int responce = http.GetResponse();
	if (responce == 302) {
		m_url = http.GetHeader(wxT("Location"));
		return DoDownload();
	}

	m_filename = wxFileName::CreateTempFileName(wxT("~"));
	wxFileOutputStream out(m_filename);
	in->Read(out);

	size_t size = in->GetSize();
	if (size != (size_t)-1 && out.GetSize() !=size) {
		wxLogError(wxT("HTTP read error, read %d of %d bytes: %s"), out.GetSize(), size, m_url.c_str());
		return false;
	}

	return true;
}

bool FbInternetBook::CheckMD5()
{
	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	bool bNotFound = true;
	if (wxZipEntry * entry = zip.GetNextEntry()) {
		bNotFound = ! zip.OpenEntry(*entry);
		delete entry;
	}
	if (bNotFound) {
		wxLogError(wxT("Entry not found: ") + m_url);
		return false;
	}

	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];
	size_t count;
	md5_context md5;
	md5_starts( &md5 );
	do {
		count = zip.Read(buf, BUFSIZE).LastRead();
		if (count) md5_update( &md5, buf, (int) count );
	} while (count);

	wxString md5sum = BaseThread::CalcMd5(md5);
	if ( md5sum != m_md5sum ) {
		wxRemoveFile(m_filename);
		wxLogError(wxT("Wrong MD5 sum: "), m_url.c_str());
		return false;
	}
	return true;
}

bool FbInternetBook::SaveFile()
{
	wxFileName zipname = m_md5sum + wxT(".zip");
	zipname.SetPath( FbStandardPaths().GetUserConfigDir() );
	wxRenameFile(m_filename, zipname.GetFullPath(), true);

	wxString sql = wxT("UPDATE states SET download=-1 WHERE md5sum=?");

	try {
		FbLocalDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_md5sum);
		stmt.ExecuteUpdate();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
		return false;
	}

	wxLogInfo(wxT("Download finished: ")+ m_url);

	FbFolderEvent(ID_UPDATE_FOLDER, 0, FT_DOWNLOAD).Post();
	return true;
}

bool FbDownloader::sm_running = false;

wxCriticalSection FbDownloader::sm_queue;

void FbDownloader::Start()
{
	wxCriticalSectionLocker locker(sm_queue);

	static bool isNull = true;
	if (isNull) {
		wxURL(strHomePage).GetProtocol().SetTimeout(10);
		wxThread * thread = new FbDownloader();
		if ( thread->Create() == wxTHREAD_NO_ERROR ) thread->Run();
		isNull = false;
	}

	sm_running = true;
}

void FbDownloader::Stop()
{
	wxCriticalSectionLocker locker(sm_queue);
	sm_running = true;
}

bool FbDownloader::IsRunning()
{
	wxCriticalSectionLocker locker(sm_queue);
	return sm_running;
}

void * FbDownloader::Entry()
{
	wxString addr;
	wxString md5sum;
	wxString ext;

	wxString sql = wxT("SELECT id, books.md5sum, file_type FROM books JOIN states ON books.md5sum=states.md5sum WHERE download>0 ORDER BY download LIMIT 1");

	while (true) {
		if (IsRunning())
			FbInternetBook().Execute();
		else
			wxSleep(3);
	}
	return NULL;
}
