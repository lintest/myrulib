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

class MyRuLibMainFrame: public wxFrame
{
	wxTextCtrl * m_FindTextCtrl;
	wxListBox * m_AuthorsListBox;
	wxListView * m_BooksListView;
	wxHtmlWindow * m_BooksInfoPanel;
	void CreateControls();
	wxToolBar * CreateButtonBar();
	wxToolBar * CreateAlphaBar(const wxString & alphabet, int toolid);
	void FillAuthorsList(const wxString & findText);
	void FillBooksList(int groupid);
    bool ParseXML(const wxString & filename);
public:
	MyRuLibMainFrame();
	bool Create(wxWindow * parent, wxWindowID id, const wxString & title);

	DECLARE_EVENT_TABLE()
	void OnExit(wxCommandEvent & event);

	void OnAuthorsListBoxSelected(wxCommandEvent & event);
	void OnBooksListViewResize(wxSizeEvent& event);
	void OnBooksListViewSelected(wxListEvent & event);
	void OnBooksInfoPanelLinkClicked(wxHtmlLinkEvent & event);
	void OnFindTextEnter( wxCommandEvent& event );
	void OnNewFile( wxCommandEvent& event );
	void OnNewDir( wxCommandEvent& event );
	virtual void OnLetterClicked( wxCommandEvent& event );
};

#endif // MYRULIBMAIN_H
