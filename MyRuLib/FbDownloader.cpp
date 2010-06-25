#include "FbDownloader.h"
#include "FbInternetBook.h"
#include "FbDatabase.h"
#include "FbDataPath.h"
#include "FbConst.h"

FbDownloader::FbDownloader()
{
	wxURL(strHomePage).GetProtocol().SetTimeout(10);
}

void * FbDownloader::Entry()
{
	while (true) {
		if (IsClosed()) break;
		wxString md5sum = GetBook();
		if (md5sum.IsEmpty()) m_condition.Wait();
		if (IsClosed()) break;
		md5sum = GetBook();
		if (md5sum.IsEmpty()) continue;
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
