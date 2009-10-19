#ifndef __BOOKSPANEL_H__
#define __BOOKSPANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbBookList.h"
#include "FbBookData.h"
#include "FbParams.h"

enum FbListMode {
    FB2_MODE_TREE = 0,
    FB2_MODE_LIST = 1,
};

class BooksPanel: public wxSplitterWindow
{
    public:
        BooksPanel();
        bool Create(wxWindow *parent, const wxSize& size, long style, int keyType, int keyMode);
    public:
        FbBookList * m_BookList;
        wxHtmlWindow * m_BookInfo;
        BookTreeItemData * GetSelectedBook();
        void EmptyBooks(const wxString title = wxEmptyString);
        void AppendBook(BookTreeItemData * data, const wxString & authors = wxEmptyString);
        void AppendAuthor(const wxString title = wxEmptyString);
        void AppendSequence(const wxString title = wxEmptyString);
        void CreateColumns(FbListMode mode);
        FbListMode GetListMode() { return m_ListMode;};
        void SetFolder(int folder) { m_folder = folder; };
        void CreateBookInfo(bool bVertical);
    private:
        void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
        wxString F(const int number);
        wxString m_AuthorName;
        FbListMode m_ListMode;
        wxTreeItemId m_AuthorItem;
        wxTreeItemId m_SequenceItem;
        int m_folder;
    private:
        void OnBooksListViewSelected(wxTreeEvent & event);
        void OnBooksListActivated(wxTreeEvent & event);
        void OnBooksListKeyDown(wxTreeEvent & event);
        void OnBooksListCollapsing(wxTreeEvent & event);
        void OnDownloadBook(wxCommandEvent & event);
        void OnEditComments(wxCommandEvent & event);
        void OnFavoritesAdd(wxCommandEvent & event);
        void OnFolderAdd(wxCommandEvent& event);
        void OnOpenAuthor(wxCommandEvent& event);
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
