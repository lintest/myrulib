#ifndef __FBFRAMEFLDR_H__
#define __FBFRAMEFLDR_H__

#include "FbFrameBase.h"

class FbFrameFldr : public FbFrameBase
{
	public:
		FbFrameFldr(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("My folders"); };
		void UpdateFolder(const int iFolder, const FbFolderType type);
	protected:
		virtual void CreateControls();
	private:
		void CreateColumns();
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
		FbParentData * m_folders;
		wxToolBar m_ToolBar;
	private:
		void OnFavoritesDel(wxCommandEvent & event);
		void OnFolderAppend(wxCommandEvent & event);
		void OnFolderModify(wxCommandEvent & event);
		void OnFolderDelete(wxCommandEvent & event);
		void OnFolderSelected(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameFldr)
};

#endif // __FBFRAMEFLDR_H__
