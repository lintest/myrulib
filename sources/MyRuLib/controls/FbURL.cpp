#include "FbURL.h"
#include "FbConst.h"
#include "FbParams.h"

FbURL::FbURL(const wxString& sUrl): wxURL(sUrl)
{
	if (FbParams::GetInt(FB_USE_PROXY))
		SetProxy(FbParams::GetStr(FB_PROXY_ADDR));

	wxHTTP & http = (wxHTTP&)GetProtocol();
	http.SetFlags(wxSOCKET_WAITALL);
	int timeout = FbParams::GetInt(FB_WEB_TIMEOUT);
	if (timeout > 0) http.SetTimeout(timeout);
	http.SetHeader(wxT("User-Agent"), MyRuLib::UserAgent());
}
