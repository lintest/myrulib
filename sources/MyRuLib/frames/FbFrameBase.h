#ifndef __FBFRAMEBASE_H__
#define __FBFRAMEBASE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/splitter.h>
#include <wx/aui/tabmdi.h>
#include "FbBookEvent.h"
#include "FbParams.h"
#include "FbWindow.h"
#include "FbMainMenu.h"
#include "FbFilterObj.h"
#include "controls/FbTreeView.h"

class FbBookPanel;

class FbFrameThread;

class FbMasterViewCtrl
	: public FbTreeViewCtrl
{
public:
	void OnCopy(wxCommandEvent & event);

	void OnSelect(wxCommandEvent & event) {}

	void OnEnableUI(wxUpdateUIEvent & event) {
		event.Enable(GetModel());
	}

	void OnDisableUI(wxUpdateUIEvent & event) {
		event.Enable(false);
	}
protected:
	DECLARE_CLASS(FbMasterViewCtrl)
	DECLARE_EVENT_TABLE()
};

class FbFrameBase :
	public wxSplitterWindow
{
public:
	FbFrameBase(wxAuiNotebook * parent, wxWindowID winid, const wxString & caption, bool select = false);

	virtual ~FbFrameBase();

	void DoEvent(wxEvent& event) {
		GetEventHashTable().HandleEvent(event, this);
	}

	virtual wxString GetTitle() { return _("Authors"); }

	public:
		virtual void UpdateMaster();
		virtual void UpdateMaster(FbMasterEvent & event);
		virtual void UpdateInfo(int id);
		virtual void UpdateFonts(bool refresh = true);
		virtual void ShowFullScreen(bool show);
		virtual void Localize(bool bUpdateMenu);
		FbListMode GetListMode();
		void RefreshBooks();
		int GetBookCount() { return m_BookCount; }
		FbBookPanel * GetBooks() { return m_BooksPanel; }
		const wxString & GetMasterFile() const { return m_MasterFile; }
	protected:
		void CreateControls(bool select);
		virtual void CreateColumns() = 0;
		void OnSubmenu(wxCommandEvent& event);
		void CreateBooksPanel(wxWindow * parent);
		int GetColOrder(int col);
		bool IsFullScreen();
		virtual void UpdateBooklist();
		virtual FbMasterInfo GetInfo();
		virtual wxString GetCountSQL() { return wxEmptyString; }
		virtual FbFrameThread * CreateCounter();
		void UpdateCounter();
	protected:
		FbTreeViewCtrl * m_MasterList;
		FbBookPanel * m_BooksPanel;
		FbFrameThread * m_MasterThread;
		FbFrameThread * m_CountThread;
		wxString m_MasterFile;
		FbFilterObj m_filter;
		int m_BookCount;
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnDirection(wxCommandEvent& event);
		void OnChangeOrder(wxCommandEvent& event);
		void OnChangeMode(wxCommandEvent& event);
		void OnShowColumns(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnFilterSet(wxCommandEvent& event);
		void OnFilterUse(wxCommandEvent& event);
		void OnFilterDel(wxCommandEvent& event);
		void OnFilterUseUpdateUI(wxUpdateUIEvent & event);
		void OnFilterDelUpdateUI(wxUpdateUIEvent & event);
		void OnDirectionUpdateUI(wxUpdateUIEvent & event);
		void OnChangeOrderUpdateUI(wxUpdateUIEvent & event);
		void OnChangeModeUpdateUI(wxUpdateUIEvent & event);
		void OnChangeViewUpdateUI(wxUpdateUIEvent & event);
		void OnMasterSelected(wxTreeEvent & event);
		void OnExportBooks(wxCommandEvent& event);
		void OnHandleMenu(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_DYNAMIC_CLASS(FbDoubleFrame)

	private:
		void OnIdleSplitter( wxIdleEvent& );
};

#endif //__FBFRAMEBASE_H__
