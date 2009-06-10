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
#include <wx/html/htmlwin.h>
#include "wx/treelistctrl.h"
#include "Authors.h"
#include "ProgressBar.h"

enum {
	ID_AUTHORS_LISTBOX = 10001,
	ID_PROGRESSBAR,
	ID_BOOKS_LISTCTRL,
	ID_BOOKS_INFO_PANEL,
	ID_NEW_FILE,
	ID_NEW_DIR,
	ID_NEW_ZIP,
	ID_FIND_TEXT,
	ID_FIND_BTN,
	ID_PROGRESS_START,
	ID_PROGRESS_UPDATE,
	ID_PROGRESS_FINISH,
};

class MyRuLibMainFrame: public wxFrame
{
	wxTextCtrl * m_FindTextCtrl;
	wxListBox * m_AuthorsListBox;
	wxTreeListCtrl * m_BooksListView;
	wxHtmlWindow * m_BooksInfoPanel;
	ProgressBar * m_ProgressBar;
	wxString m_StatusText;
	void CreateControls();
	wxToolBar * CreateButtonBar();
	wxToolBar * CreateAlphaBar(const wxString & alphabet, int toolid);
	void FillAuthorsList(const wxString & findText);
	void FillAuthorsList(AuthorsRowSet * allAuthors);
    bool ParseXML(const wxString & filename);
public:
	MyRuLibMainFrame();
	bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
	void OnExit(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnAuthorsListBoxSelected(wxCommandEvent & event);
	void OnBooksListViewResize(wxSizeEvent& event);
	void OnBooksListViewSelected(wxTreeEvent & event);
	void OnBooksListActivated(wxTreeEvent & event);
	void OnBooksListKeyDown(wxTreeEvent & event);
	void OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event);
	void OnFindTextEnter( wxCommandEvent& event );
	void OnNewFile( wxCommandEvent& event );
	void OnNewDir( wxCommandEvent& event );
	void OnNewZip( wxCommandEvent& event );
	void OnProgressStart(wxCommandEvent& event);
	void OnProgressUpdate(wxCommandEvent& event);
	void OnProgressFinish(wxCommandEvent& event);
	virtual void OnLetterClicked( wxCommandEvent& event );
	DECLARE_EVENT_TABLE()
};

#endif // MYRULIBMAIN_H
