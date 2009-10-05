#ifndef __FBCONST_H__
#define __FBCONST_H__

#include <wx/wx.h>

enum {
    ID_AUTHORS_LISTBOX = wxID_HIGHEST + 1,
    ID_PROGRESSBAR,
    ID_BOOKS_LISTCTRL,
    ID_BOOKS_INFO_PANEL,
    ID_FIND_AUTHOR,
    ID_FIND_TITLE,
    ID_PROGRESS_START,
    ID_PROGRESS_UPDATE,
    ID_PROGRESS_FINISH,
    ID_MODE_TREE,
    ID_MODE_LIST,
    ID_SPLIT_HORIZONTAL,
    ID_SPLIT_VERTICAL,
    ID_FB2_ONLY,
    ID_FOLDER,
    ID_ERROR,
    ID_LOG_TEXTCTRL,
    ID_BOOKINFO_UPDATE,
    ID_SEARCH_BOOK,
    ID_OPEN_WEB,
    ID_FRAME_AUTHORS,
    ID_FRAME_GENRES,
    ID_FRAME_FAVOUR,
    ID_UNSELECTALL,
    ID_OPEN_BOOK,
    ID_MENU_AUTHOR,
    ID_MENU_TITLE,
    ID_MENU_SEARCH,
    ID_MENU_DB_INFO,
    ID_MENU_VACUUM,
    ID_GENRES_TREE,
    ID_EMPTY_BOOKS,
    ID_APPEND_BOOK,
    ID_APPEND_AUTHOR,
    ID_APPEND_SEQUENCE,
    ID_FOLDER_LIST,
	ID_APPEND_FOLDER,
	ID_MODIFY_FOLDER,
	ID_DELETE_FOLDER,
	ID_UPDATE_FOLDER,
    ID_FAVORITES_DEL,
    ID_LETTER_RU = wxID_HIGHEST + 100,
    ID_LETTER_EN = wxID_HIGHEST + 200,
    ID_FAVORITES_ADD = wxID_HIGHEST + 300,
};

extern const wxString strVersionInfo;
extern const wxString strHomePage;

extern const wxString alphabetRu;
extern const wxString alphabetEn;
extern const wxString strAlphabet;
extern const wxString strNobody;
extern const wxString strRusJE;
extern const wxString strRusJO;
extern const wxString strOtherSequence;
extern const wxString strBookNotFound;

extern const int ciNoAuthor;
extern const int ciMaxImageWidth;

#endif // __FBCONST_H__
