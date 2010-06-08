#ifndef __FBFRAMEAUTHOR_H__
#define __FBFRAMEAUTHOR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbAuthList.h"

enum FbAuthorListMode
{
	FB_AUTHOR_MODE_CHAR,
	FB_AUTHOR_MODE_TEXT,
	FB_AUTHOR_MODE_CODE,
};

class FbFrameAuthor : public FbFrameBase
{
	public:
		FbFrameAuthor(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Authors"); };
		void FindAuthor(const wxString &text);
		void OpenAuthor(const int author, const int book);
		void SelectRandomLetter();
		void ActivateAuthors();
		virtual void ShowFullScreen(bool show);
		virtual void Localize(bool bUpdateMenu);
	protected:
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString) { return NULL; };
		virtual void CreateControls();
		virtual void UpdateBooklist();
		virtual wxMenuBar * CreateMenuBar();
	private:
		wxToolBar * CreateAlphaBar(wxWindow * parent, wxWindowID id, const wxString & alphabet, const int &toolid, long style);
		void ToggleAlphabar(const int &idLetter);
		BookTreeItemData * GetSelectedBook();
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void CreateMasterThread();
		void CreateColumns();
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxToolBar * m_RuAlphabar;
		wxToolBar * m_EnAlphabar;
	private:
		FbAuthListInfo m_info;
	private:
		void OnMasterSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnAllClicked(wxCommandEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnExternal(wxCommandEvent& event);
		void OnViewAlphavet(wxCommandEvent& event);
		void OnViewAlphavetUpdateUI(wxUpdateUIEvent & event);
		void OnMasterAppend(wxCommandEvent& event);
		void OnMasterModify(wxCommandEvent& event);
		void OnMasterDelete(wxCommandEvent& event);
		void OnMasterReplace(wxCommandEvent& event);
		void OnMasterPage(wxCommandEvent& event);
		void OnMasterPageUpdateUI(wxUpdateUIEvent & event);
		void OnContextMenu(wxTreeEvent& event);
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameAuthor)
	protected:
		class MasterMenu: public wxMenu
		{
			public:
				MasterMenu(int id);
		};
	private:
		class MenuBar: public FbFrameMenu
		{
			public:
				MenuBar();
			private:
				class MenuView: public FbFrameMenu::MenuView {
					public: MenuView();
				};
		};
		class MenuMaster: public FbMenu {
			public: MenuMaster();
		};
};

#endif // __FBFRAMEAUTHOR_H__
