#ifndef __FBSTRING_H__
#define __FBSTRING_H__

#include <wx/wx.h>

class FbString: public wxString  {
	public: 
		FbString(const char * psz): wxString(psz, wxConvUTF8) {}
		wxString static T(const char * psz) { return wxGetTranslation(FbString(psz)); }
};

#define fbS(str) (FbString(str))

#define fbT(str) (FbString::T(str))

#endif // __FBSTRING_H__
