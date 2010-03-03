#ifndef __FBFRAMEAUTHOR_H__
#define __FBFRAMEAUTHOR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"

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
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxToolBar * m_RuAlphabar;
		wxToolBar * m_EnAlphabar;
	private:
		FbAuthorListMode m_AuthorMode;
		wxString m_AuthorText;
		int m_AuthorCode;
	private:
		void OnMasterSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnAllClicked(wxCommandEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnExternal(wxCommandEvent& event);
		void OnCharEvent(wxKeyEvent& event);
		void OnViewAlphavet(wxCommandEvent& event);
		void OnViewAlphavetUpdateUI(wxUpdateUIEvent & event);
		void OnMasterAppend(wxCommandEvent& event);
		void OnMasterModify(wxCommandEvent& event);
		void OnMasterDelete(wxCommandEvent& event);
		void OnMasterReplace(wxCommandEvent& event);
		void OnMasterPage(wxCommandEvent& event);
		void OnMasterPageUpdateUI(wxUpdateUIEvent & event);
		void OnContextMenu(wxTreeEvent& event);
		DECLARE_EVENT_TABLE()
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
