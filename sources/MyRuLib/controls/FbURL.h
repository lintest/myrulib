#ifndef __FBURL_H__
#define __FBURL_H__

#include <wx/url.h>

class FbURL: public wxURL
{
	public:
		FbURL(const wxString& sUrl = wxEmptyString);
	private:
		static wxString GetUserAgent();
};

#endif // __FBURL_H__