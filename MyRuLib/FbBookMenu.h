#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/arrimpl.cpp>

class FbMasterData;

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
		FbBookMenu(int id, const FbMasterData &data, bool bShowOrder);
	private:
		int m_id;
};

class FbMenuAuthors: public wxMenu
{
	public:
		static int GetAuthor(const int id);
		static void Connect(wxWindow * frame, wxObjectEventFunction func);
	public:
		FbMenuAuthors(int book);
	private:
		static FbMenuAuthorArray sm_authors;
};

class FbMenuFolders: public wxMenu
{
	public:
		static void Init();
		static void EmptyFolders() { sm_folders.Empty(); };
		static size_t GetCount();
		static void LoadFolders();
		static int GetFolder(const int id);
		static void Connect(wxWindow * frame, wxObjectEventFunction func);
	public:
		FbMenuFolders(const FbMasterData &data);
	private:
		static FbMenuFolderArray sm_folders;
};

#endif // __FBBOOKMENU_H__
