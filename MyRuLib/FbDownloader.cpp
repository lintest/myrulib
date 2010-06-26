#include "FbDownloader.h"
#include "FbInternetBook.h"
#include "FbDatabase.h"
#include "FbDataPath.h"
#include "FbConst.h"

FbDownloader::FbDownloader(): m_condition(m_mutex), m_closed(false)
{
	wxURL(strHomePage).GetProtocol().SetTimeout(10);
}

void FbDownloader::Signal() 
{ 
	wxMutexLocker locker(m_mutex);
	m_condition.Broadcast();
}

void FbDownloader::Close() 
{ 
	wxMutexLocker locker(m_mutex);
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
		m_condition.Wait();
		wxMutexLocker locker(m_mutex);
		if (m_closed) break;
		wxString md5sum = GetBook();
		if (md5sum.IsEmpty()) continue;
		if (m_closed) break;
		try { FbInternetBook(this, md5sum).Execute(); } catch (...) {}
		wxSleep(3);
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
