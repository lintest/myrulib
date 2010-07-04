#ifndef __FBFRAMEDOWNLD_H__
#define __FBFRAMEDOWNLD_H__

#include "FbFrameBase.h"

class FbFrameDownld : public FbFrameBase
{
	public:
		FbFrameDownld(wxAuiMDIParentFrame * parent);
		wxString GetTitle() const { return _("Downloads"); };
		void UpdateFolder(const int iFolder, const FbFolderType type);
	protected:
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString);
		virtual void CreateControls();
	private:
		void CreateColumns();
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
	private:
		void OnFavoritesDel(wxCommandEvent & event);
		void OnFolderAppend(wxCommandEvent & event);
		void OnFolderModify(wxCommandEvent & event);
		void OnFolderDelete(wxCommandEvent & event);
		void OnStart(wxCommandEvent & event);
		void OnPause(wxCommandEvent & event);
		void OnFolderSelected(wxTreeEvent & event);
		void OnMoveUp(wxCommandEvent& event);
		void OnMoveDown(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameDownld)
};

#endif // __FBFRAMEDOWNLD_H__
