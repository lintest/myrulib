#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/arrimpl.cpp>

#define fbNO_FOLDER (-999)

class FbMenuFolderItem
{
	public:
		int id;
		int folder;
		wxString name;
};

class FbMenuAuthorItem
{
	public:
		int id;
		int author;
};

WX_DECLARE_OBJARRAY(FbMenuFolderItem, FbMenuFolderArray);

WX_DECLARE_OBJARRAY(FbMenuAuthorItem, FbMenuAuthorArray);

class FbBookMenu: public wxMenu
{
	public:
		FbBookMenu(int id, int iFolder);
		static void EmptyFolders() { sm_folders.Empty(); };
		static int GetFolder(const int id);
		static int GetAuthor(const int id);
		static void ConnectFolders(wxWindow * frame, wxObjectEventFunction func);
		static void ConnectAuthors(wxWindow * frame, wxObjectEventFunction func);
	private:
		static void LoadFolders();
		wxMenu * CreateAuthorMenu();
	private:
		static FbMenuFolderArray sm_folders;
		static FbMenuAuthorArray sm_authors;
		int m_id;
};

#endif // __FBBOOKMENU_H__
