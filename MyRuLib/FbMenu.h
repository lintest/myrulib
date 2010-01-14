#ifndef __FBMENU_H__
#define __FBMENU_H__

#include <wx/wx.h>
#include <wx/artprov.h>

class FbMenu: public wxMenu
{
	public:
		wxMenuItem * AppendImg(int itemid, const wxString& text, const wxArtID& art, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL)
		{
			wxMenuItem * item = new wxMenuItem(this, itemid, text, help, kind);
			item->SetBitmap( wxArtProvider::GetBitmap(art) );
			return Append(item);
		};
};

class FbMenuSort: public FbMenu
{
	public:
		FbMenuSort();
};

class FbMenuRating: public FbMenu
{
	public:
		FbMenuRating();
};

#endif // __FBMENU_H__
