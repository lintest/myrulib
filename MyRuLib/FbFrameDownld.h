#ifndef __FBFRAMEDOWNLD_H__
#define __FBFRAMEDOWNLD_H__

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

class FbFrameDownld : public FbFrameBase
{
	public:
		FbFrameDownld(wxAuiMDIParentFrame * parent);
		void UpdateFolder(const int iFolder, const FbFolderType type);
	protected:
		virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		void CreateBookInfo();
		void FillFolders(const int iCurrent = 0);
		void FillByFolder(FbFolderData * data);
		void DeleteItems(const wxTreeItemId &root, wxArrayInt &items);
		FbFolderData * GetSelected();
	private:
		wxToolBar * m_toolbar;
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
	protected:
		class DownldThread: public BaseThread
		{
			public:
				DownldThread(FbFrameBase * frame, FbListMode mode, FbFolderData * data)
					:BaseThread(frame, mode), m_folder(data->GetId()), m_number(sm_skiper.NewNumber()), m_type(data->GetType()) {};
				virtual void *Entry();
			protected:
				virtual wxString GetOrder();
			private:
				static FbThreadSkiper sm_skiper;
				int m_folder;
				int m_number;
				FbFolderType m_type;
		};
};

#endif // __FBFRAMEDOWNLD_H__
