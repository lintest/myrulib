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

class FbFrameAuthor : public FbFrameBase
{
public:
	FbFrameAuthor() :FbFrameBase() {};
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
	FbAuthorList * m_AuthorList;
	wxSplitterWindow * m_BooksSplitter;
	wxToolBar * m_RuAlphabar;
	wxToolBar * m_EnAlphabar;
private:
	void OnAuthorSelected(wxTreeEvent & event);
	void OnLetterClicked(wxCommandEvent& event);
	void OnExternal(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEAUTHOR_H__
