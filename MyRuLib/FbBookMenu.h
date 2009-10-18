#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/arrimpl.cpp>

#define fbNO_FOLDER (-999)

class FbFolderItem
{
	public:
		int id;
		int folder;
		wxString name;
};

WX_DECLARE_OBJARRAY(FbFolderItem, FbFolderArray);

class FbBookMenu: public wxMenu
{
	public:
		FbBookMenu(int id, int iFolder);
		static void EmptyFolders() { sm_folders.Empty(); };
		static int GetFolder(const int id);
		static void Connect(wxWindow * frame, wxObjectEventFunction func);
	private:
		static void LoadFolders();
	private:
		static FbFolderArray sm_folders;
		int m_id;
};

#endif // __FBBOOKMENU_H__
