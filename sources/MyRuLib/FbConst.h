#ifndef __FBCONST_H__
#define __FBCONST_H__

#include <wx/wx.h>

#define fbLIST_CACHE_SIZE   64
#define fbLIST_ARRAY_SIZE 1024

enum {
	ID_MASTER_LIST = wxID_HIGHEST + 1,
	ID_BOOKS_LISTCTRL,
	ID_BOOKS_INFO_PANEL,
	ID_PROGRESSBAR,
	ID_SEQUENCE_FIND,
	ID_AUTHOR_TXT,
	ID_AUTHOR_BTN,
	ID_TITLE_TXT,
	ID_TITLE_BTN,
	ID_INIT_LETTER,
	ID_CHOICE_LETTER,
	ID_PROGRESS_START,
	ID_PROGRESS_UPDATE,
	ID_PROGRESS_PULSE,
	ID_MODE_TREE,
	ID_MODE_LIST,
	ID_SPLIT_HORIZONTAL,
	ID_SPLIT_VERTICAL,
	ID_SPLIT_NOTHING,
	ID_FILTER_SET,
	ID_FILTER_USE,
	ID_ERROR,
	ID_LOG_TEXTCTRL,
	ID_FULLSCREEN,
	ID_BOOK_PREVIEW,
	ID_AUTH_PREVIEW,
	ID_UPDATE_BOOK,
	ID_SEARCH_BOOK,
	ID_OPEN_WEB,
	ID_FRAME_AUTH, // Save and restore if (ID_FRAME_AUTHOR <= ID < ID_FRAME_SEARCH)
	ID_FRAME_SEQN,
	ID_FRAME_GENR,
	ID_FRAME_FLDR,
	ID_FRAME_DOWN,
	ID_FRAME_DATE,
	ID_FRAME_ARCH,
	ID_FRAME_FIND, // Save and restore if (ID_FRAME_AUTHOR <= ID < ID_FRAME_SEARCH)
	ID_FRAME_INFO,
	ID_FRAME_HTML,
	ID_UNSELECTALL,
	ID_UPDATE_FONTS,
	ID_OPEN_BOOK,
	ID_MENU_SEARCH,
	ID_MENU_AUTHOR,
	ID_MENU_DB_INFO,
	ID_MENU_GENLIST,
	ID_MENU_DB_OPEN,
	ID_MENU_OPTIMIZE,
	ID_MENU_VACUUM,
	ID_MENU_CONFIG,
	ID_MENU_VIEW,
	ID_MENU_UPDATE,
	ID_SHOW_COLUMNS,
	ID_ALPHABET_RU,
	ID_ALPHABET_EN,
	ID_EMPTY_MASTERS,
	ID_APPEND_MASTER,
	ID_EMPTY_BOOKS,
	ID_APPEND_BOOK,
	ID_APPEND_AUTHOR,
	ID_APPEND_SEQUENCE,
	ID_AUTHOR_INFO,
	ID_SEQUENCE_TXT,
	ID_SEQUENCE_BTN,
	ID_INIT_SEARCH,
	ID_FOUND_NOTHING,
	ID_EDIT_COMMENTS,
	ID_APPEND_FOLDER,
	ID_MODIFY_FOLDER,
	ID_DELETE_FOLDER,
	ID_UPDATE_FOLDER,
	ID_UPDATE_MASTER,
	ID_HTML_DOCUMENT,
	ID_HTML_CAPTION,
	ID_HTML_COMMENT,
	ID_HTML_SUBMIT,
	ID_HTML_MODIFY,
	ID_BOOK_PAGE,
	ID_BOOK_AUTHOR,
	ID_BOOK_SEQUENCE,
	ID_DOWNLOAD_BOOK,
	ID_SYSTEM_DOWNLOAD,
	ID_DELETE_DOWNLOAD,
	ID_DATABASE_INFO,
	ID_FAVORITES_DEL,
	ID_START,
	ID_PAUSE,
	ID_MASTER_APPEND,
	ID_MASTER_MODIFY,
	ID_MASTER_REPLACE,
	ID_MASTER_DELETE,
	ID_MASTER_PAGE,
	ID_BOOKS_COUNT,
	ID_RATING_0,
	ID_RATING_1,
	ID_RATING_2,
	ID_RATING_3,
	ID_RATING_4,
	ID_RATING_5,
	ID_ART_DEFAULT,
	ID_ART_COMPACT,
	ID_ART_STANDART,
	ID_ART_BUTTONS,
	ID_ART_TOOLBAR,
	ID_ART_VSTUDIO,
	ID_ART_MOZILLA,
	ID_WINDOW_NEXT,
	ID_WINDOW_PREV,
	ID_DIRECTION,
	ID_ORDER_MENU,
	ID_ORDER_AUTHOR,
	ID_ORDER_RATING,
	ID_ORDER_GENRE,
	ID_ORDER_LANG,
	ID_SCRIPT_RUN,
	ID_SCRIPT_LOG,
	ID_SCRIPT_ERROR,
	ID_SCRIPT_EXIT,
	ID_MODEL_CREATE,
	ID_MODEL_APPEND,
	ID_FAVORITES_ADD, // Always the last !!!
};

class MyRuLib
{
public:
	static wxString ProgramName();
	static wxString ProgramInfo();
	static wxString VersionInfo();
	static wxString HomePage();
	static wxString MailAddr();
	static wxString UserAgent();
};

extern const wxString alphabetRu;
extern const wxString alphabetEn;
extern const wxString strAlphabet;
extern const wxString strRusJE;
extern const wxString strRusJO;

extern const wxString strUpdateAuthorCount;
extern const wxString strUpdateSequenCount;

#define COLON wxString(wxT(": "))

void FbLogMessage(const wxString &info, const wxString &text);

void FbLogError(const wxString &info, const wxString &text);

void FbLogWarning(const wxString &info, const wxString &text);

void FbMessageBox(const wxString &info, const wxString &text);

wxString GetRatingText(int index);

#endif // __FBCONST_H__
