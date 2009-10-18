#ifndef __FBFRAMEBASEMENU_H__
#define __FBFRAMEBASEMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"

class FbFrameBaseMenu: public wxMenuBar
{
	public:
		FbFrameBaseMenu();
	protected:
		virtual FbMenu * GetBookMenu();
		virtual FbMenu * GetViewMenu();
};

#endif // __FBFRAMEBASEMENU_H__
