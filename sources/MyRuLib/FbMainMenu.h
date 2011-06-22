#ifndef __FBMAINMENU_H__
#define __FBMAINMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"

class FbMenuBar: public wxMenuBar
{
	public:
		FbMenuBar();
		
	protected:
		class MenuFile: public FbMenu {
			public: MenuFile();
		};

		class MenuEdit: public FbMenu {
			public: MenuEdit();
		};

		class MenuFrame: public FbMenu {
			public: MenuFrame();
		};

		class MenuLib: public FbMenu {
			public:
				MenuLib();
		};

		class MenuBook: public FbMenu {
			public: MenuBook();
		};

		class MenuHelp: public FbMenu {
			public: MenuHelp();
		};

		class MenuRecent: public FbMenu {
			public: MenuRecent();
		};

		class MenuWindow: public FbMenu {
			public: MenuWindow();
		};

		class MenuListMode: public FbMenu {
			public: MenuListMode();
		};

		class MenuTabArt: public FbMenu {
			public: MenuTabArt();
		};

		class MenuPreview: public FbMenu {
			public: MenuPreview();
		};

};

#endif // __FBMAINMENU_H__
