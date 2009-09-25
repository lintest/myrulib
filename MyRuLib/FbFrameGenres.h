#ifndef __FBFRAMEGENRES_H__
#define __FBFRAMEGENRES_H__

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
#include "BookListCtrl.h"

class FbFrameGenres : public FbFrameBase
{
public:
    static void Execute(wxAuiMDIParentFrame * parent, const wxString &title);
	FbFrameGenres(wxAuiMDIParentFrame * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString);
    BookListCtrl * GetBooks() { return FbFrameBase::m_BooksPanel.m_BookList; };
protected:
	virtual wxToolBar *CreateToolBar(long style, wxWindowID winid, const wxString& name);
	virtual void CreateControls();
private:
    void CreateBookInfo();
	void FillBooks(const wxString & code);
private:
    BookListCtrl * m_GenresList;
private:
    void OnExternal(wxCommandEvent& event);
    void OnGenreSelected(wxTreeEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEGENRES_H__