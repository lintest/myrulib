#ifndef __FBCONST_H__
#define __FBCONST_H__

#include <wx/wx.h>

enum {
    ID_AUTHORS_LISTBOX = 20001,
    ID_PROGRESSBAR,
    ID_BOOKS_LISTCTRL,
    ID_BOOKS_INFO_PANEL,
    ID_FIND_AUTHOR,
    ID_FIND_TITLE,
    ID_PROGRESS_START,
    ID_PROGRESS_UPDATE,
    ID_PROGRESS_FINISH,
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
    ID_UNSELECTALL,
    ID_OPEN_BOOK,
    ID_MENU_AUTHOR,
    ID_MENU_TITLE,
    ID_MENU_SEARCH,
    ID_MENU_DB_INFO,
    ID_MENU_VACUUM,
    ID_LETTER_RU = 30100,
    ID_LETTER_EN = 30200,
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
