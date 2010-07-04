#ifndef __FBFRAMEFOLDER_H__
#define __FBFRAMEFOLDER_H__

#include "FbFrameBase.h"

class FbFrameFolder : public FbFrameBase
{
	public:
		FbFrameFolder(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("My folders"); };
		void UpdateFolder(const int iFolder, const FbFolderType type);
		virtual void ShowFullScreen(bool show);
	protected:
		virtual void CreateControls();
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString);
	private:
		void CreateColumns();
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
		FbParentData * m_folders;
	private:
		void OnFavoritesDel(wxCommandEvent & event);
		void OnFolderAppend(wxCommandEvent & event);
		void OnFolderModify(wxCommandEvent & event);
		void OnFolderDelete(wxCommandEvent & event);
		void OnFolderSelected(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameFolder)
};

#endif // __FBFRAMEFOLDER_H__
