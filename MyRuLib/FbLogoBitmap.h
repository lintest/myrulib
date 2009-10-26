#ifndef __FBLOGOBITMAP_H__
#define __FBLOGOBITMAP_H__

#include "res/nocheck.xpm"
#include "res/checked.xpm"
#include "res/checkout.xpm"

#ifndef __WIN32__
#include "res/home.xpm"
#endif

#include <wx/wx.h>

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
		};
};

#endif // __FBLOGOBITMAP_H__
