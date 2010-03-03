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

#if wxCHECK_VERSION(2, 9, 0)
	result = AppendPathComponent(result,wxT(""));
#else
	result = AppendAppName(result);
#endif

	if (!wxFileName::DirExists(result)) wxFileName::Mkdir(result);

	return result;
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
