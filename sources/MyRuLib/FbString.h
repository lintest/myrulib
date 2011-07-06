#ifndef __FBSTRING_H__
#define __FBSTRING_H__

#include <wx/wx.h>

class FbString: public wxString  {
	public: 
		FbString(const char * psz): wxString(psz, wxConvUTF8) {}
		wxString Translate() { return wxGetTranslation(*this); }
};

wxString FbTransl(const char * psz);

#define fbT(str) (FbString(str))

#define T(str) (FbTransl(str))

#endif // __FBSTRING_H__
