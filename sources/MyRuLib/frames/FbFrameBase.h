#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include "FbBookPanel.h"
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbWindow.h"
#include "FbThread.h"
#include "FbMainMenu.h"
#include "FbFilterObj.h"
#include "controls/FbTreeView.h"

class FbFrameBase : public FbAuiMDIChildFrame
{
	public:
		FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
		~FbFrameBase();
		virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString) { return NULL; }
		void UpdateMaster(FbMasterEvent & event);
		void UpdateInfo(int id);
		virtual void UpdateFonts(bool refresh = true);
		wxString GetFilterSQL() { return m_filter.GetSQL(); };
		FbListMode GetListMode() { return m_BooksPanel->GetListMode(); };
		void UpdateStatus();
		virtual void ShowFullScreen(bool show);
		virtual void Localize(bool bUpdateMenu);
		FbBookPanel * GetBooks() { return m_BooksPanel; }
		void RefreshBooks() { if (m_BooksPanel && m_BooksPanel->m_BookList) m_BooksPanel->m_BookList->Refresh(); }
	protected:
		virtual void CreateControls();
		virtual void CreateColumns() = 0;
		virtual wxMenuBar * CreateMenuBar();
		void OnSubmenu(wxCommandEvent& event);
		void CreateBooksPanel(wxWindow * parent);
		int GetColOrder(int col);
		bool IsFullScreen();
		virtual void UpdateBooklist();
		virtual FbMasterInfo GetInfo();
	protected:
		FbTreeViewCtrl * m_MasterList;
		FbBookPanel * m_BooksPanel;
		FbThread * m_MasterThread;
		FbFilterObj m_filter;
		int m_BookCount;
	private:
		void OnActivated(wxActivateEvent & event);
		void OnBooksCount(FbCountEvent& event);
		void OnDirection(wxCommandEvent& event);
		void OnChangeOrder(wxCommandEvent& event);
		void OnChangeMode(wxCommandEvent& event);
		void OnShowColumns(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnFilterSet(wxCommandEvent& event);
		void OnFilterUse(wxCommandEvent& event);
		void OnFilterUseUpdateUI(wxUpdateUIEvent & event);
		void OnMenuOrderUpdateUI(wxUpdateUIEvent & event);
		void OnDirectionUpdateUI(wxUpdateUIEvent & event);
		void OnChangeOrderUpdateUI(wxUpdateUIEvent & event);
		void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
		void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
		void OnMasterSelected(wxTreeEvent & event);
		void OnExportBooks(wxCommandEvent& event);
		void OnEmptyBooks(wxCommandEvent& event);
		void OnHandleMenu(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameBase)

	private:
		class MenuBar: public FbFrameMenu
		{
			public:
				MenuBar();
		};
};

#endif //__FBFRAMEBASE_H__

