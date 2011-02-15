#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/arrimpl.cpp>
#include "controls/FbTreeModel.h"

class FbMasterInfo;

#include <wx/hashmap.h>
WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, FbMenuMap);

class FbBookMenu: public wxMenu
{
	public:
		enum FbMenuType {
			MenuAuth = 0,
			MenuSeqn,
			MenuFldr,
			MenuCount,
		};
		static bool GetKey(int id, int &key, FbMenuType &type);
		static int SetKey(int key, FbMenuType type);
		FbBookMenu(wxWindow * frame, FbModelItem item, int book);
		void Init(const FbMasterInfo &master, bool bShowOrder);
	private:
		void AppendAuthorsMenu();
		void AppendSeriesMenu();
		void AppendFoldersMenu(int folder);
	private:
		static FbMenuMap sm_key;
		static FbMenuMap sm_type;
		static int sm_next;
		wxWindow * m_frame;
		int m_auth;
		int m_seqn;
		int m_book;
};

class FbMenuAuthors: public wxMenu
{
	public:
		FbMenuAuthors(int book, wxWindow * frame, wxObjectEventFunction func);
};

class FbMenuSeries: public wxMenu
{
	public:
		FbMenuSeries(int book, wxWindow * frame, wxObjectEventFunction func);
};

class FbMenuFolders: public wxMenu
{
	public:
		FbMenuFolders(int folder, wxWindow * frame, wxObjectEventFunction func);
};

#endif // __FBBOOKMENU_H__
