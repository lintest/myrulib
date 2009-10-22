#ifndef __FBMAINMENU_H__
#define __FBMAINMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"

class FbMainMenu: public wxMenuBar
{
	public:
		FbMainMenu();
	protected:
		virtual FbMenu * GetBookMenu() { return NULL; };
		virtual FbMenu * GetViewMenu() { return NULL; };
};

#endif // __FBMAINMENU_H__
