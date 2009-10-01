#ifndef __BOOKSPANEL_H__
#define __BOOKSPANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "BookListCtrl.h"
#include "FbBookData.h"

class BooksPanel: public wxSplitterWindow
{
    public:
        BooksPanel();
        BooksPanel(wxWindow *parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_3D,
                     long substyle = 0,
                     const wxString& name = wxT("bookspanel"));
        bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_3D,
                     long substyle = 0,
                     const wxString& name = wxT("bookspanel"));
    public:
        BookListCtrl * m_BookList;
        wxHtmlWindow * m_BookInfo;
        BookTreeItemData * GetSelectedBook();
        void EmptyBooks(const wxString title = wxEmptyString);
    private:
        void CreateBookInfo();
        void ShowContextMenu(const wxPoint& pos);
        wxString m_AuthorName;
    private:
        void OnBooksListViewSelected(wxTreeEvent & event);
        void OnBooksListActivated(wxTreeEvent & event);
        void OnBooksListKeyDown(wxTreeEvent & event);
        void OnBooksListCollapsing(wxTreeEvent & event);
        void OnChangeView(wxCommandEvent & event);
        void OnFavoritesAdd(wxCommandEvent & event);
        void OnOpenBook(wxCommandEvent & event);
        void OnImageClick(wxTreeEvent &event);
        void OnInfoUpdate(wxCommandEvent& event);
        void OnSubmenu(wxCommandEvent& event);
        void OnContextMenu(wxTreeEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        void OnUnselectAll(wxCommandEvent& event);
        DECLARE_EVENT_TABLE();
};


#endif // __BOOKSPANEL_H__
