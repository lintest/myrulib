#ifndef __FBMENU_H__
#define __FBMENU_H__

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/hashmap.h>

class FbMenu: public wxMenu
{
	public:
		enum Type {
			AUTH,
			SEQN,
			FLDR,
			CLSS,
		};

		wxMenuItem * AppendImg(int itemid, const wxString& text, const wxArtID& art, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL)
		{
			wxMenuItem * item = new wxMenuItem(this, itemid, text, help, kind);
			item->SetBitmap( wxArtProvider::GetBitmap(art) );
			return Append(item);
		}

		wxMenuItem * AppendSub(Type type, int code, const wxString& name, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL);
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

class FbMenuRefs: public FbMenu
{
	public:
		static wxMenuItem * Create(wxMenu * menu);
		FbMenuRefs() {}
};

class FbMenuData {
	public:
		FbMenu::Type type;
		int code;
};

WX_DECLARE_HASH_MAP(int, FbMenuData, wxIntegerHash, wxIntegerEqual, FbMenuHash);

class FbMenuItem: public wxMenuItem
{
	public:
		static bool Get(wxWindowID id, FbMenu::Type & type, int & code);

		FbMenuItem(wxMenu * menu, FbMenu::Type type, int code, 
			const wxString& name = wxEmptyString, 
			const wxString& help = wxEmptyString, 
			wxItemKind kind = wxITEM_NORMAL, 
			wxMenu * submenu = NULL 
		);

		virtual ~FbMenuItem();

	private:
		static wxWindowID NewId(FbMenu::Type type, int code);
		static FbMenuHash m_hash;
};

#endif // __FBMENU_H__
