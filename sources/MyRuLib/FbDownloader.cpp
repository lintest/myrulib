#include "FbDownloader.h"
#include "FbInternetBook.h"
#include "FbDatabase.h"
#include "FbDataPath.h"
#include "FbParams.h"
#include "FbConst.h"

FbDownloader::FbDownloader(): m_condition(m_mutex), m_closed(false)
{
	wxURL(MyRuLib::HomePage()).GetProtocol().SetTimeout(FbParams::GetInt(FB_WEB_TIMEOUT));
}

void FbDownloader::Signal()
{
	wxCriticalSectionLocker locker(m_section);
	m_condition.Broadcast();
}

void FbDownloader::Close()
{
	wxCriticalSectionLocker locker(m_section);
	m_closed = true;
	m_condition.Broadcast();
}

void FbDownloader::Execute()
{
	if ( Create() == wxTHREAD_NO_ERROR ) Run();
}

bool FbDownloader::IsClosed()
{
	return m_closed;
}

void * FbDownloader::Entry()
{
	while (true) {
		while (true) {
			wxString md5sum = GetBook();
			if (md5sum.IsEmpty()) break;
			if (m_closed) return NULL;
			try { FbInternetBook(this, md5sum).Execute(); } catch (...) {}
			wxSleep(3);
		}
		{
			wxMutexLocker locker(m_mutex);
			m_condition.Wait();
			if (m_closed) return NULL;
		}
	}
	return NULL;
}

wxString FbDownloader::GetBook()
{
	wxString sql = wxT("SELECT md5sum FROM states WHERE download<0 ORDER BY download DESC");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	if (result.NextRow()) return result.GetString(0);
	return wxEmptyString;
}

wxString FbDownloader::GetFilename(const wxString &md5sum, bool bCreateFolder)
{
	wxString path = FbParams::GetPath(FB_DOWNLOAD_DIR);

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
