#ifndef __FBDATAPATH_H__
#define __FBDATAPATH_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

class FbStandardPaths: public wxStandardPaths
{
	public:
		wxString GetDefaultName() const;
		virtual wxString GetConfigFile() const;
		virtual wxString GetUserConfigDir() const;
		static wxString MakeRelative(const wxString &fullname, const wxString &basefile);
};

#endif // __FBDATAPATH_H__
