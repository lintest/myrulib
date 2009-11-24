#include "FbDataPath.h"
#include "MyRuLibApp.h"
#include "FbParams.h"

wxString FbStandardPaths::GetUserConfigDir() const
{
#if defined(__WIN32__)
	wxString result = wxStandardPaths::GetUserConfigDir();
#else
	wxString result = wxStandardPaths::GetUserConfigDir() + wxT("/.config/");
#endif

	if (!wxFileName::DirExists(result)) wxFileName::Mkdir(result);

	result = AppendAppName(result);
	if (!wxFileName::DirExists(result)) wxFileName::Mkdir(result);

	return result;
}

wxString FbStandardPaths::GetDataFile() const
{
	wxFileName filename = GetDatabaseFilename();
	filename.Normalize();
	return filename.GetFullPath();
}

wxFileName FbStandardPaths::GetDatabaseFilename() const
{
	wxFileName filename = GetExecutablePath();
	filename.SetExt(wxT("db"));

	if (wxGetApp().argc > 1) {
		wxString arg = wxGetApp().argv[1];
		if (wxFileName::DirExists(arg)) {
			wxFileName filename = wxGetApp().GetAppName() + wxT(".db");
			filename.SetPath(arg);
			return filename;
		}
		return wxFileName(arg);
	}

	if (filename.FileExists()) return filename;

	filename.SetPath(GetUserConfigDir());
	return filename;
}

wxString FbStandardPaths::GetConfigFile() const
{
	wxFileName filename = GetExecutablePath();
	filename.SetExt(wxT("cfg"));
	if ( !filename.FileExists() ) filename.SetPath(GetUserConfigDir());
	filename.Normalize();
	return filename.GetFullPath();
}

wxString FbStandardPaths::GetDownloadDir(bool bMustExist) const
{
	wxString path = FbParams::GetText(FB_DOWNLOAD_DIR);
	if ( bMustExist && !wxFileName::DirExists(path))
		wxFileName::Mkdir(path, 0777, wxPATH_MKDIR_FULL);
	return path;
}
