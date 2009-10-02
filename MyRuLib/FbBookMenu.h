#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>

class FbBookMenu: public wxMenu
{
	public:
		FbBookMenu(int id, bool bFavorites);
	private:
		int m_id;
};

#endif // __FBBOOKMENU_H__
