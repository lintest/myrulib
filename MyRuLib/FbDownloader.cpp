#include "FbDownloader.h"
#include "MyRuLibApp.h"
#include "FbBookEvent.h"
#include "InfoCash.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbDatabase.h"
#include "BaseThread.h"
#include "polarssl/md5.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include "FbDataPath.h"

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

wxString FbInternetBook::GetURL(const int id)
{
	return wxT("http://") + FbParams::GetText(DB_DOWNLOAD_HOST) + wxString::Format(wxT("/b/%d/download"), id);
}

FbInternetBook::FbInternetBook(const wxString& md5sum)
	: m_id(0), m_md5sum(md5sum), m_zipped(false)
{
	wxString sql = wxT("SELECT id, file_type FROM books WHERE md5sum=? AND id>0");
	try {
		FbCommonDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, md5sum);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) {
			m_id = result.GetInt(0);
			m_filetype = result.GetString(1);
			m_url = GetURL(m_id);
		}
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}
}

bool FbInternetBook::Execute()
{
	bool result = m_id && DoDownload();
	SaveFile(result);
	return result;
}

bool FbInternetBook::DoDownload()
{
	wxString user = FbParams::GetText(DB_DOWNLOAD_USER);
	if ( user.IsEmpty() ) return DownloadUrl();

	wxString host = FbParams::GetText(DB_DOWNLOAD_HOST);
	wxString pass = FbParams::GetText(DB_DOWNLOAD_PASS);
	wxString addr = wxString::Format(wxT("http://%s/b/%d/get?destination=b/%d/get"), host.c_str(), m_id, m_id);
	wxLogInfo(_("Download: ") + addr);

	FbURL url(addr);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("URL error: ") + m_url);
		return false;
	}
	wxHTTP & http = (wxHTTP&)url.GetProtocol();
    http.SetTimeout(10);
    http.SetHeader(wxT("Content-type"), wxT("application/x-www-form-urlencoded"));
    wxString buffer = wxString::Format(wxT("form_id=user_login_block&name=%s&pass=%s"), user.c_str(), pass.c_str());
    http.SetPostBuffer(buffer);

	wxInputStream * in = url.GetInputStream();
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("Connect error: ") + m_url);
		return false;
	}

	wxString cookie = http.GetHeader(wxT("Set-Cookie")).BeforeFirst(wxT(';'));
	if (http.GetResponse() == 302) {
		m_url = http.GetHeader(wxT("Location"));
		wxLogInfo(_("Redirect") + COLON + m_url);
		return DownloadUrl(cookie);
	}

	bool ok = ReadFile(in);
	if ( !ok ) { wxLogError(_("Authentication failure: ") + m_url); }
	return ok;
}

bool FbInternetBook::DownloadUrl(const wxString &cookie)
{
	FbURL url(m_url);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("URL error") + COLON + m_url);
		return false;
	}
	wxHTTP & http = (wxHTTP&)url.GetProtocol();
	if ( !cookie.IsEmpty() ) http.SetHeader(wxT("Cookie"), cookie);

	wxInputStream * in = url.GetInputStream();
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("Connect error") + COLON + m_url);
		return false;
	}
	if (http.GetResponse() == 302) {
		m_url = http.GetHeader(wxT("Location"));
 		wxLogInfo(_("Redirect") + COLON + m_url);
		return DownloadUrl(cookie);
	}
	return ReadFile(in);
}

bool FbInternetBook::ReadFile(wxInputStream * in)
{
	m_filename = wxFileName::CreateTempFileName(wxT("~"));
	wxFileOutputStream out(m_filename);

	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];
	size_t size = in->GetSize() ? in->GetSize() : 0xFFFFFF;
	size_t count = 0;
	size_t pos = 0;

	bool zipped = false;
	md5_context md5;
	md5_starts( &md5 );
	do {
		FbProgressEvent(ID_PROGRESS_UPDATE, m_url, pos/(size/1000), _("File download")).Post();
		count = in->Read(buf, BUFSIZE).LastRead();
		if ( count ) md5_update( &md5, buf, (int) count );
		if ( pos==0 && count>1 && buf[0]=='P' && buf[1]=='K') zipped = true;
		out.Write(buf, count);
		pos += count;
	} while (count);
	FbProgressEvent(ID_PROGRESS_UPDATE).Post();

	if (size != (size_t)-1 && out.GetSize() !=size) {
		wxLogError(_("HTTP read error, read %d of %d bytes: %s"), out.GetSize(), size, m_url.c_str());
		return false;
	}

	wxString md5sum = BaseThread::CalcMd5(md5);
	if ( md5sum == m_md5sum )
		return true;
	else if ( zipped )
		return CheckZip();

	wxLogError(_("Download error: %s"), m_url.c_str());
	return false;
}

bool FbInternetBook::CheckZip()
{
	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	bool bNotFound = true;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		bool ok = (entry->GetInternalName().Right(4).Lower() != wxT(".fbd"));
		if (ok) bNotFound = ! zip.OpenEntry(*entry);
		delete entry;
		if (ok) break;
	}
	if (bNotFound) {
		wxLogError(_("Zip read error: ") + m_url);
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
	if ( md5sum == m_md5sum ) {
		m_zipped = true;
	} else {
		wxLogError(_("Wrong MD5 sum: "), m_url.c_str());
		return false;
	}
	return true;
}

void FbInternetBook::SaveFile(const bool success)
{
	if (success) {
		wxFileName zipname = FbDownloader::GetFilename(m_md5sum, true);
		if (m_zipped) zipname.SetExt(wxT("zip"));
		wxRenameFile(m_filename, zipname.GetFullPath(), true);
	} else {
		wxRemoveFile(m_filename);
	}

	wxString sql = wxT("UPDATE states SET download=? WHERE md5sum=?");

	try {
		FbLocalDatabase database;
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, success ? -1 : -2);
		stmt.Bind(2, m_md5sum);
		stmt.ExecuteUpdate();
	} catch (wxSQLite3Exception & e) {
		wxLogError(e.GetMessage());
	}

	wxLogInfo(_("Download finished: ")+ m_url);

	InfoCash::EmptyInfo(m_id);

	FbFolderEvent(ID_UPDATE_FOLDER, 0, FT_DOWNLOAD).Post();
	FbCommandEvent(fbEVT_BOOK_ACTION, ID_UPDATE_BOOK, m_id).Post();
}

bool FbDownloader::sm_running = false;

wxCriticalSection FbDownloader::sm_queue;

wxArrayString FbDownloader::sm_waitings;

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

void FbDownloader::Pause()
{
	wxCriticalSectionLocker locker(sm_queue);
	sm_running = false;
}

bool FbDownloader::IsRunning()
{
	wxCriticalSectionLocker locker(sm_queue);
	return sm_running;
}

void FbDownloader::Push(const wxString & md5sum)
{
	wxCriticalSectionLocker locker(sm_queue);
	if (sm_waitings.Index(md5sum) == wxNOT_FOUND) sm_waitings.Add(md5sum);
}

void * FbDownloader::Entry()
{
	wxString addr;
	wxString md5sum;
	wxString ext;

	while (true) {
		if (IsRunning()) try {
			wxArrayString md5sum;
			GetBooklist(md5sum);
			size_t count = md5sum.Count();
			if (!count) Pause();
			for (size_t i=0; i<count; i++) {
				FbInternetBook(md5sum[i]).Execute();
			}
		} catch (...) {};
		wxSleep(3);
	}
	return NULL;
}

void FbDownloader::GetBooklist(wxArrayString &md5sum)
{
	wxString sql = wxT("SELECT md5sum FROM states WHERE download>0 ORDER BY download");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while ( result.NextRow() ) {
		md5sum.Add( result.GetString(0) );
	}
}

wxString FbDownloader::GetFilename(const wxString &md5sum, bool bCreateFolder)
{
	wxString path = FbStandardPaths().GetDownloadDir(false);

	wxString name = md5sum;
	for (int i=1; i<=3; i++) {
		path += wxFileName::GetPathSeparator();
		path += name.Left(2);
		name = name.Mid(2);
	}

	if ( bCreateFolder && !wxFileName::DirExists(path))
		wxFileName::Mkdir(path, 0777, wxPATH_MKDIR_FULL);

	path += wxFileName::GetPathSeparator();
	path += name;

	return path;
}
