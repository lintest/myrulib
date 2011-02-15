#ifndef __FBURL_H__
#define __FBURL_H__

#include <wx/url.h>

class FbURL: public wxURL
{
	public:
		FbURL(const wxString& sUrl = wxEmptyString);
};

#endif // __FBURL_H__