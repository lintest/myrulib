#ifndef __FBBOOKMENU_H__
#define __FBBOOKMENU_H__

#include <wx/wx.h>
#include "FbMenu.h"
#include "controls/FbTreeModel.h"

class FbMasterInfo;

class FbBookMenu: public FbMenu
{
	public:
		FbBookMenu(FbModelItem item, int book);
		void Init(const FbMasterInfo &master, bool bShowOrder);
	private:
		void AppendAuth();
		void AppendSeqn();
		void AppendFldr(int folder);
	private:
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
