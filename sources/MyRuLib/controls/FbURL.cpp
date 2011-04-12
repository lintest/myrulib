#include "FbURL.h"
#include "FbParams.h"

FbURL::FbURL(const wxString& sUrl): wxURL(sUrl)
{
	if (FbParams::GetInt(FB_USE_PROXY))
		SetProxy(FbParams::GetStr(FB_PROXY_ADDR));

	GetProtocol().SetTimeout(FbParams::GetInt(FB_WEB_TIMEOUT));
}

