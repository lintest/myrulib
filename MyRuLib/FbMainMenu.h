#ifndef __FBMAINMENU_H__
#define __FBMAINMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"

class FbMenuBar: public wxMenuBar
{
	protected:

		class MenuFrame: public FbMenu {
			public: MenuFrame();
		};

		class MenuLib: public FbMenu {
			public:
				MenuLib();
		};

		class MenuSetup: public FbMenu {
			public:
				MenuSetup();
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

		class MenuLocalize: public FbMenu {
			public: MenuLocalize();
		};
};

class FbMainMenu: public FbMenuBar
{
	public:
		FbMainMenu();

	protected:

		class MenuFile: public FbMenu {
			public: MenuFile();
		};

		class MenuView: public FbMenu {
			public: MenuView();
		};

};

class FbFrameMenu: public FbMenuBar
{
	public:
		FbFrameMenu() {};

	protected:

		class MenuFile: public FbMenu {
			public: MenuFile();
		};

		class MenuBook: public FbMenu {
			public: MenuBook();
		};

		class MenuView: public FbMenu {
			public: MenuView();
		};
};

#endif // __FBMAINMENU_H__
