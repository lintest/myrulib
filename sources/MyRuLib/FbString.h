#ifndef __FBSTRING_H__
#define __FBSTRING_H__

#include <wx/wx.h>

class FbString: public wxString  {
	public: 
		FbString(const char * psz): wxString(psz, wxConvUTF8) {}
		wxString Translate() const { return wxGetTranslation(*this); }
};

#define fbT(str) (FbString(str))

#endif // __FBSTRING_H__
