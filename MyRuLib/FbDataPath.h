#ifndef __FBDATAPATH_H__
#define __FBDATAPATH_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

class FbStandardPaths: public wxStandardPaths
{
	public:
		virtual wxString GetDataFile() const;
		virtual wxString GetConfigFile() const;
		virtual wxString GetAppFileName() const;
		virtual wxString GetUserConfigDir() const;
		virtual wxString GetDownloadDir(bool bMustExist = false) const;
	private:
		wxFileName GetDatabaseFilename() const;
};

#endif // __FBDATAPATH_H__
