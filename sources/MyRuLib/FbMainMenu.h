#ifndef __FBMAINMENU_H__
#define __FBMAINMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"

#define fbFIND_MENU_POSITION 3
#define fbBOOK_MENU_POSITION 4

class FbMenuBar: public wxMenuBar
{
	public:
		FbMenuBar();
		class MenuFile     : public FbMenu { public: MenuFile     ();};
		class MenuEdit     : public FbMenu { public: MenuEdit     ();};
		class MenuFrame    : public FbMenu { public: MenuFrame    ();};
		class MenuLib      : public FbMenu { public: MenuLib      ();};
		class MenuTree     : public FbMenu { public: MenuTree     ();};
		class MenuHelp     : public FbMenu { public: MenuHelp     ();};
		class MenuRecent   : public FbMenu { public: MenuRecent   ();};
		class MenuWindow   : public FbMenu { public: MenuWindow   ();};
		class MenuListMode : public FbMenu { public: MenuListMode ();};
		class MenuTabArt   : public FbMenu { public: MenuTabArt   ();};
		class MenuPreview  : public FbMenu { public: MenuPreview  ();};

	#ifdef FB_INCLUDE_READER
		class MenuRead     : public FbMenu { public: MenuRead     ();};
	#endif // FB_INCLUDE_READER
};

#endif // __FBMAINMENU_H__
