#ifndef __FBFRAMEAUTHOR_H__
#define __FBFRAMEAUTHOR_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbAuthorList.h"
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
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
	private:
		wxToolBar * CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style);
		void ToggleAlphabar(const int &idLetter);
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
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
		void OnColClick(wxListEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnExternal(wxCommandEvent& event);
		void OnCharEvent(wxKeyEvent& event);
		void OnEmptyAuthors(wxCommandEvent& event);
		void OnAppendAuthor(FbAuthorEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEAUTHOR_H__
