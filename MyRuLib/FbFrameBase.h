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
#include "FbMasterData.h"
#include "FbMasterList.h"

class FbFrameBase : public FbAuiMDIChildFrame
{
	public:
		FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
		virtual bool Create(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
		virtual wxToolBar * CreateToolBar(long style, wxWindowID winid, const wxString& name);
		void UpdateInfo(int id);
		virtual void UpdateFonts(bool refresh = true);
		wxString GetOrderSQL() { return m_BooksPanel->GetOrderSQL(); };
		wxString GetFilterSQL() { return m_filter.GetSQL(); };
		FbListMode GetListMode() { return m_BooksPanel->GetListMode(); };
		void SetMasterData(FbMasterData const * data) { m_BooksPanel->SetMasterData(data); };
		FbMasterData const * GetMasterData() { return m_BooksPanel->GetMasterData(); };
		void UpdateStatus();
		virtual void ShowFullScreen(bool show);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist() = 0;
		virtual wxMenuBar * CreateMenuBar();
		virtual wxString GetStatus();
		int GetModeKey();
		int GetViewKey();
		void OnSubmenu(wxCommandEvent& event);
		void CreateBooksPanel(wxWindow * parent, long substyle);
		int GetColOrder(int col);
		wxString Naming(int count, const wxString &single, const wxString &genitive, const wxString &plural);
		int GetBookCount();
		bool IsFullScreen();
	protected:
		FbMasterList * m_MasterList;
		FbBookPanel * m_BooksPanel;
		wxToolBar * m_ToolBar;
		FbFilterObj m_filter;
	private:
		void OnActivated(wxActivateEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnDirection(wxCommandEvent& event);
		void OnChangeOrder(wxCommandEvent& event);
		void OnChangeMode(wxCommandEvent& event);
		void OnShowColumns(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnFilterUse(wxCommandEvent& event);
		void OnFilterNot(wxCommandEvent& event);
		void OnFilterUseUpdateUI(wxUpdateUIEvent & event);
		void OnMenuOrderUpdateUI(wxUpdateUIEvent & event);
		void OnDirectionUpdateUI(wxUpdateUIEvent & event);
		void OnChangeOrderUpdateUI(wxUpdateUIEvent & event);
		void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
		void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
		void OnExternal(wxCommandEvent& event);
		void OnAppendBook(FbBookEvent& event);
		void OnAppendAuthor(wxCommandEvent& event);
		void OnAppendSequence(wxCommandEvent& event);
		void OnEmptyBooks(wxCommandEvent& event);
		void OnTreeCollapsing(wxTreeEvent & event);
		DECLARE_EVENT_TABLE()

	private:
		class MenuBar: public FbFrameMenu
		{
			public:
				MenuBar();
		};
};

#endif //__FBFRAMEBASE_H__

