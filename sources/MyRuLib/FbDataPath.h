#ifndef __FBDATAPATH_H__
#define __FBDATAPATH_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

class FbStandardPaths: public wxStandardPaths
{
	public:
		virtual wxString GetConfigFile() const;
		virtual wxString GetUserConfigDir() const;
		virtual wxString GetDownloadDir(bool bMustExist = false) const;
};

#endif // __FBDATAPATH_H__
