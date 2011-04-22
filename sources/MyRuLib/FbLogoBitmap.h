#ifndef __FBLOGOBITMAP_H__
#define __FBLOGOBITMAP_H__

#include "res/nocheck.xpm"
#include "res/checked.xpm"
#include "res/checkout.xpm"
#include "res/add.xpm"
#include "res/mod.xpm"
#include "res/del.xpm"

#ifndef __WIN32__
#include "res/home.xpm"
#endif

#include <wx/wx.h>

#define FB_CHECKBOX_HEIGHT 14
#define FB_CHECKBOX_WIDTH  14

class FbLogoBitmap: public wxBitmap
{
	public:
		FbLogoBitmap()
			#ifndef __WIN32__
			:wxBitmap(home_xpm)
			#endif
		{
			#ifdef __WIN32__
			wxIcon icon(wxT("aaaa"));
			CopyFromIcon(icon);
			#endif
		};

	private:
		void Unused()
		{
			wxUnusedVar(nocheck_xpm);
			wxUnusedVar(checked_xpm);
			wxUnusedVar(checkout_xpm);
			wxUnusedVar(add_xpm);
			wxUnusedVar(mod_xpm);
			wxUnusedVar(del_xpm);
		};
};

#endif // __FBLOGOBITMAP_H__
