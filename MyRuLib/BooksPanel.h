#ifndef __BOOKSPANEL_H__
#define __BOOKSPANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "BookListCtrl.h"
#include "FbBookData.h"

enum FbListMode {
    FB2_MODE_TREE = 0,
    FB2_MODE_LIST = 1,
};

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
        void AppendBook(BookTreeItemData * data, const wxString & authors = wxEmptyString);
        void AppendAuthor(const wxString title = wxEmptyString);
        void AppendSequence(const wxString title = wxEmptyString);
        void CreateColumns(FbListMode mode);
        FbListMode GetListMode() { return m_ListMode;};
    private:
        void CreateBookInfo();
        void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
        wxString m_AuthorName;
        FbListMode m_ListMode;
        wxTreeItemId m_AuthorItem;
        wxTreeItemId m_SequenceItem;
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
