#include "FbURL.h"
#include "FbConst.h"
#include "FbParams.h"
#include "wx/base64.h"

FbURL::FbURL(const wxString& sUrl): wxURL(sUrl)
{
	bool use_proxy = FbParams::GetInt(FB_USE_PROXY);
	if (use_proxy) SetProxy(FbParams::GetStr(FB_PROXY_ADDR));

	wxHTTP & http = (wxHTTP&)GetProtocol();
	http.SetFlags(wxSOCKET_WAITALL);
	int timeout = FbParams::GetInt(FB_WEB_TIMEOUT);
	if (timeout > 0) http.SetTimeout(timeout);
	http.SetHeader(wxT("User-Agent"), MyRuLib::UserAgent());

	if (use_proxy) {
		wxString auth = FbParams::GetStr(FB_PROXY_USER);
		if (!auth.IsEmpty()) {
			auth << wxT(':') << FbParams::GetStr(FB_PROXY_PASS);
			wxCharBuffer buff = auth.mb_str(wxConvUTF8);
			wxString text = wxT("Basic"); 
			text << wxT(' ') << wxBase64Encode(buff, strlen(buff));
			http.SetHeader(wxT("Proxy-Authorization"), text);
		}
	}
}
