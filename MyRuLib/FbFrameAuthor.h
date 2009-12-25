#ifndef __FBFRAMEAUTHOR_H__
#define __FBFRAMEAUTHOR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbManager.h"

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
		void FindAuthor(const wxString &text);
		void OpenAuthor(const int author, const int book);
		void SelectRandomLetter();
		void ActivateAuthors();
		virtual void ShowFullScreen(bool show);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
		virtual wxMenuBar * CreateMenuBar();
	private:
		wxToolBar * CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style);
		void ToggleAlphabar(const int &idLetter);
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxToolBar * m_RuAlphabar;
		wxToolBar * m_EnAlphabar;
	private:
		FbAuthorListMode m_AuthorMode;
		wxString m_AuthorText;
		int m_AuthorCode;
	private:
		void OnAuthorSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnColClick(wxListEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnExternal(wxCommandEvent& event);
		void OnCharEvent(wxKeyEvent& event);
		void OnEmptyAuthors(wxCommandEvent& event);
		void OnAppendAuthor(FbAuthorEvent& event);
		void OnMasterAppend(wxCommandEvent& event);
		void OnMasterModify(wxCommandEvent& event);
		void OnMasterDelete(wxCommandEvent& event);
		void OnMasterReplace(wxCommandEvent& event);
		void OnContextMenu(wxTreeEvent& event);
		DECLARE_EVENT_TABLE()
	protected:
		class AuthorThread: public BaseThread
		{
			public:
				AuthorThread(FbFrameBase * frame, FbListMode mode, const int author)
					:BaseThread(frame, mode), m_author(author), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			protected:
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual wxString GetSQL(const wxString & condition);
			private:
				static FbThreadSkiper sm_skiper;
				int m_author;
				int m_number;
		};
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
		};
		class MenuMaster: public FbMenu {
			public: MenuMaster();
		};
};

#endif // __FBFRAMEAUTHOR_H__
