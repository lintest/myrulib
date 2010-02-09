#ifndef __FBFRAMEFOLDER_H__
#define __FBFRAMEFOLDER_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/wxsqlite3.h>
#include "wx/treelistctrl.h"
#include "FbFrameBase.h"
#include "FbTreeListCtrl.h"
#include "FbBookEvent.h"

class FbFrameFolder : public FbFrameBase
{
	public:
		FbFrameFolder(wxAuiMDIParentFrame * parent);
		void UpdateFolder(const int iFolder, const FbFolderType type);
		virtual void ShowFullScreen(bool show);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
	private:
		wxToolBar * m_FolderBar;
	private:
		void OnFavoritesDel(wxCommandEvent & event);
		void OnFolderAppend(wxCommandEvent & event);
		void OnFolderModify(wxCommandEvent & event);
		void OnFolderDelete(wxCommandEvent & event);
		void OnFolderSelected(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEFOLDER_H__
