/***************************************************************
 * Name:      MyRuLibMain.h
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef MYRULIBMAIN_H
#define MYRULIBMAIN_H

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "wx/treelistctrl.h"
#include "ProgressBar.h"
#include "BookListCtrl.h"

class MyRuLibMainFrame: public wxFrame
{
public:
    enum {
        ID_AUTHORS_LISTBOX = 10001,
        ID_PROGRESSBAR,
        ID_BOOKS_LISTCTRL,
        ID_BOOKS_INFO_PANEL,
        ID_NEW_ZIP,
        ID_REG_ZIP,
        ID_FIND_TEXT,
        ID_FIND_BTN,
        ID_PROGRESS_START,
        ID_PROGRESS_UPDATE,
        ID_PROGRESS_FINISH,
		ID_SPLIT_HORIZONTAL,
		ID_SPLIT_VERTICAL,
		ID_EXTERNAL,
		ID_FB2_ONLY,
    };
private:
	wxTextCtrl * m_FindTextCtrl;
	wxListBox * m_AuthorsListBox;
	BookListCtrl * m_BooksListView;
	wxHtmlWindow * m_BooksInfoPanel;
	ProgressBar * m_ProgressBar;
	wxSplitterWindow * m_books_splitter;
	wxString m_StatusText;
	wxToolBar * m_toolBar;
	void CreateControls();
	wxToolBar * CreateButtonBar();
	wxToolBar * CreateAlphaBar(const wxString & alphabet, int toolid);
    bool ParseXML(const wxString & filename);
	void SelectFirstAuthor();
	void CreateBookInfo();
private:
	int GetSelectedBook();
	bool vertical;
public:
	MyRuLibMainFrame();
private:
	bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
	void OnExit(wxCommandEvent & event);
    void OnSetup(wxCommandEvent & event);
	void OnChangeView(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnAuthorsListBoxSelected(wxCommandEvent & event);
	void OnBooksListViewResize(wxSizeEvent& event);
	void OnBooksListViewSelected(wxTreeEvent & event);
	void OnBooksListActivated(wxTreeEvent & event);
	void OnBooksListKeyDown(wxTreeEvent & event);
	void OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event);
	void OnFindTextEnter( wxCommandEvent& event );
	void OnNewZip( wxCommandEvent& event );
	void OnRegZip( wxCommandEvent& event );
	void OnProgressStart(wxCommandEvent& event);
	void OnProgressUpdate(wxCommandEvent& event);
	void OnProgressFinish(wxCommandEvent& event);
	void OnImageClick(wxTreeEvent &event);
    void OnInfoUpdate(wxCommandEvent& event);
    void OnChangeFilter(wxCommandEvent& event);
    void OnExternal(wxCommandEvent& event);
	virtual void OnLetterClicked( wxCommandEvent& event );
	DECLARE_EVENT_TABLE()
};

#endif // MYRULIBMAIN_H
