#ifndef _AUI_DOCVIEW_APP_H
#define _AUI_DOCVIEW_APP_H

#include <wx/wx.h>

class AUIDocViewApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

DECLARE_APP(AUIDocViewApp);

#endif
