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
    FbFrameAuthor(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
    void FindAuthor(const wxString &text);
    void SelectRandomLetter();
    void ActivateAuthors();
protected:
	virtual void CreateControls();
private:
	wxToolBar * CreateAlphaBar(wxWindow * parent, const wxString & alphabet, const int &toolid, long style);
	void ToggleAlphabar(const int &idLetter);
	void SelectFirstAuthor();
	BookTreeItemData * GetSelectedBook();
private:
	FbAuthorList * m_AuthorsListBox;
	wxSplitterWindow * m_BooksSplitter;
    wxToolBar * m_RuAlphabar;
    wxToolBar * m_EnAlphabar;
private:
	void OnAuthorsListBoxSelected(wxCommandEvent & event);
	void OnLetterClicked(wxCommandEvent& event);
    void OnExternal(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEAUTHOR_H__
