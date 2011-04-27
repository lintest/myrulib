#include "FbInternetBook.h"
#include "FbImportReader.h"
#include "MyRuLibApp.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbConst.h"
#include "FbParams.h"
#include "FbDatabase.h"
#include "polarssl/md5.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include "FbDataPath.h"
#include "FbDateTime.h"
#include "controls/FbURL.h"

wxString FbInternetBook::GetURL(const int id, const wxString& md5sum)
{
	wxString host = FbParams::GetStr(DB_DOWNLOAD_HOST);
	if (FbParams::IsGenesis()) {
		wxString key = md5sum.IsEmpty() ? FbCommonDatabase().GetMd5(id) : md5sum;
		wxString addr = wxT("http://%s/get?nametype=orig&md5=%s");
		return wxString::Format(addr, host.c_str(), key.c_str());
	} else {
		wxString addr = wxT("http://%s/b/%d/download");
		return wxString::Format(addr, host.c_str(), id);
	}
}

bool FbInternetBook::Download(const wxString & address, wxString & filename, const wxString &cookie)
{
	filename = wxFileName::CreateTempFileName(wxT("fb"));
	return Download(wxGetApp().GetTopWindow(), address, filename, cookie);
}

bool FbInternetBook::Download(wxEvtHandler * owner, const wxString & address, const wxString & filename, const wxString &cookie)
{
	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	bool ok = false;
	int timeout = FbParams::GetInt(FB_WEB_TIMEOUT);

	wxString addr = address;
	int step = FbParams::GetInt(FB_WEB_ATTEMPT);
	while (step--) {

		FbURL url(addr);

		wxHTTP & http = (wxHTTP&)url.GetProtocol();
		if ( !cookie.IsEmpty() ) http.SetHeader(wxT("Cookie"), cookie);

		wxInputStream * in = url.GetInputStream();
		switch ( http.GetResponse() / 100 ) {
			case 3: {
				addr = http.GetHeader(wxT("Location"));
				FbLogWarning(_("Redirect"), addr);
				continue;
			} break;
			case 4: {
				FbLogError(_("File is missing"), addr);
				return false;
			} break;
			case 5: {
				FbLogError(_("Server error"), addr);
				continue;
			} break;
			default: if (in == NULL) {
				FbLogError(_("Download error"), addr);
				continue;
			}
		}

		size_t offset = 0;
		size_t size = in->GetSize();
		wxFileOutputStream out(filename);
		while (true) {
			FbProgressEvent(ID_PROGRESS_UPDATE, addr, offset * 1000 / size, _("File download")).Post(owner);
			size_t count = in->Read(buf, BUFSIZE).LastRead();
			if ( count ) {
				out.Write(buf, count);
				offset += count;
			} else break;
		} 
		FbProgressEvent(ID_PROGRESS_UPDATE).Post(owner);
		if (ok = offset == size) break;
		addr = address;
	} 

	return ok;
}

FbInternetBook::FbInternetBook(FbDownloader * owner, const wxString& md5sum)
	: m_id(0), m_owner(owner), m_md5sum(md5sum), m_zipped(false)
{
	wxString sql = wxT("SELECT id, file_type FROM books WHERE md5sum=? AND id>0");

	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, md5sum);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	if ( result.NextRow() ) {
		m_id = result.GetInt(0);
		m_filetype = result.GetString(1);
		m_url = GetURL(m_id, m_md5sum);
	}
}

bool FbInternetBook::Execute()
{
	bool ok = m_id && DoDownload();
	if (ok) ok = CheckFile();
	SaveFile(ok);
	return ok;
}

bool FbInternetBook::DoDownload()
{
	wxString user = FbParams::GetStr(DB_DOWNLOAD_USER);
	if ( user.IsEmpty() ) return Download(m_url, m_filename);

	wxString host = FbParams::GetStr(DB_DOWNLOAD_HOST);
	wxString pass = FbParams::GetStr(DB_DOWNLOAD_PASS);
	wxString addr = wxString::Format(wxT("http://%s/b/%d/get?destination=b/%d/get"), host.c_str(), m_id, m_id);
	FbLogMessage(_("Download"), addr);

	FbURL url(addr);
	if (url.GetError() != wxURL_NOERR) {
		FbLogError(_("URL error"), m_url);
		return false;
	}
	wxHTTP & http = (wxHTTP&)url.GetProtocol();
    http.SetTimeout(FbParams::GetInt(FB_WEB_TIMEOUT));
    http.SetHeader(wxT("Content-type"), wxT("application/x-www-form-urlencoded"));
    wxString buffer = wxString::Format(wxT("form_id=user_login_block&name=%s&pass=%s"), user.c_str(), pass.c_str());
    http.SetPostBuffer(buffer);

	if (m_owner->IsClosed()) return false;

	wxInputStream * in = url.GetInputStream();
	if (url.GetError() != wxURL_NOERR) {
		FbLogError(_("Connect error"), m_url);
		return false;
	}

	wxString cookie = http.GetHeader(wxT("Set-Cookie")).BeforeFirst(wxT(';'));
	if (http.GetResponse() / 100 == 3) {
		m_url = http.GetHeader(wxT("Location"));
		FbLogMessage(_("Redirect"), m_url);
	}
	return Download(m_url, m_filename, cookie);
}

bool FbInternetBook::CheckFile()
{
	wxFileInputStream in(m_filename);

	const size_t BUFSIZE = 1024;
	unsigned char buf[BUFSIZE];

	size_t pos = 0;
	size_t count = 0;
	bool zipped = false;

	md5_context md5;
	md5_starts( &md5 );
	do {
		size_t count = in.Read(buf, BUFSIZE).LastRead();
		if ( count ) {
			md5_update( &md5, buf, (int) count );
			if ( pos == 0 && count > 1 && memcmp(buf, "PK", 2) == 0) zipped = true;
			pos += count;
		}
	} while (count);

	wxString md5sum = Md5(md5);
	if ( md5sum == m_md5sum ) {
		return true;
	} else if ( zipped ) {
		return CheckZip();
	}

	FbLogError(_("Download error"), m_url);
	return false;
}

bool FbInternetBook::CheckZip()
{
	wxFFileInputStream in(m_filename);
	wxZipInputStream zip(in);

	bool bNotFound = true;
	while (wxZipEntry * entry = zip.GetNextEntry()) {
		bool ok = (Ext(entry->GetInternalName()) != wxT("fbd"));
		if (ok) bNotFound = ! zip.OpenEntry(*entry);
		delete entry;
		if (ok) break;
	}
	if (bNotFound) {
		FbLogError(_("Can't open zip"), m_url);
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

	wxString md5sum = Md5(md5);
	if ( md5sum == m_md5sum ) {
		m_zipped = true;
	} else {
		FbLogError(_("Wrong MD5 sum"), m_url);
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

	if (m_owner->IsClosed()) return;

	wxString sql = wxT("UPDATE states SET download=? WHERE md5sum=?");

	int code = success ? FbDateTime::Today().Code() : 1;
	FbLocalDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, code);
	stmt.Bind(2, m_md5sum);
	stmt.ExecuteUpdate();

	if (m_owner->IsClosed()) return;

	if (success) {
		FbCollection::ResetBook(m_id);
		FbCollection::ResetInfo(m_id);
		FbFolderEvent(ID_UPDATE_FOLDER, 0, FT_DOWNLOAD).Post();
		FbCommandEvent(fbEVT_BOOK_ACTION, ID_UPDATE_BOOK, m_id).Post();
		FbLogMessage(_("Download finished"), m_url);
	}

	{
		FbMasterDownInfo info = FbMasterDownInfo::DT_WAIT;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, false).Post();
	}

	{
		FbMasterDownInfo info = success ? FbMasterDownInfo::DT_READY : FbMasterDownInfo::DT_ERROR;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, true).Post();
	}
}

