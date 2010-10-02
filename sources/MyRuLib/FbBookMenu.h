#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/arrimpl.cpp>
#include "FbTreeModel.h"

class FbMasterInfo;

#include <wx/hashmap.h>
WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, FbMenuMap);

class FbBookMenu: public wxMenu
{
	public:
		static int GetKey(int id);
		static int SetKey(int key);
		FbBookMenu(wxWindow * frame, int book): m_frame(frame), m_book(book) {}
		void Init(const FbMasterInfo &master, bool bShowOrder);
	public:
		wxObjectEventFunction m_fldr_func;
		wxObjectEventFunction m_auth_func;
		wxObjectEventFunction m_seqn_func;
	private:
		void AppendAuthorsMenu();
		void AppendSeriesMenu();
		void AppendFoldersMenu(int folder);
	private:
		static FbMenuMap sm_map;
		static int sm_next;
		wxWindow * m_frame;
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
