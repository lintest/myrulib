#ifndef __FBCONST_H__
#define __FBCONST_H__

#include <wx/wx.h>

#define fbLIST_CACHE_SIZE   64
#define fbLIST_ARRAY_SIZE 1024

#define fbCOLLATE_CYR wxString(wxT(" COLLATE CYR"))

enum {
	// Controls
	ID_MYRULIB = wxID_HIGHEST + 1000,
	ID_MASTER_FIND,
	ID_MASTER_LIST,
	ID_BOOKLIST_CTRL,
	ID_PREVIEW_CTRL,
	ID_TEXTLOG_CTRL,
	ID_AUI_NOTEBOOK,
	ID_PROGRESSBAR,
	ID_AUTHOR_TXT,
	ID_AUTHOR_BTN,
	ID_TITLE_TXT,
	ID_TITLE_BTN,
	ID_INIT_LETTER,

	// Frames
	ID_FRAME_AUTH, // Save and restore if (ID_FRAME_AUTH <= ID < ID_FRAME_FIND)
	ID_FRAME_SEQN,
	ID_FRAME_GENR,
	ID_FRAME_FLDR,
	ID_FRAME_DOWN,
	ID_FRAME_DATE,
	ID_FRAME_ARCH,
	ID_FRAME_FIND, // Save and restore if (ID_FRAME_AUTH <= ID < ID_FRAME_FIND)
	ID_FRAME_NODE,
	ID_FRAME_CLSS,
	ID_FRAME_INFO,
	ID_FRAME_HTML,
	ID_FRAME_READ,

	// Events
	ID_PROGRESS_START,
	ID_PROGRESS_UPDATE,
	ID_PROGRESS_PULSE,
	ID_PROGRESS_FINISH,

	// Global menu
	ID_MENU_SEARCH,
	ID_MENU_GENRES,
	ID_MENU_DB_INFO,
	ID_MENU_DB_OPEN,
	ID_MENU_OPTIMIZE,
	ID_MENU_VACUUM,
	ID_MENU_CONFIG,
	ID_MENU_UPDATE,
	ID_FULLSCREEN,
	ID_ART_DEFAULT,
	ID_ART_COMPACT,
	ID_ART_STANDART,
	ID_ART_BUTTONS,
	ID_ART_TOOLBAR,
	ID_ART_VSTUDIO,
	ID_ART_MOZILLA,
	ID_WINDOW_NEXT,
	ID_WINDOW_PREV,
	ID_TEXTLOG_SHOW,
	ID_TEXTLOG_HIDE,
	ID_STATUS_SHOW,

	// First submenu ID
	ID_SUBMENU_FIRST,

	// Frame menu
	ID_EMPTY_MASTERS,
	ID_APPEND_MASTER,
	ID_AUTHOR_INFO,
	ID_SPLIT_HORIZONTAL,
	ID_SPLIT_VERTICAL,
	ID_SPLIT_NOTHING,
	ID_MODE_TREE,
	ID_MODE_LIST,
	ID_BOOK_PREVIEW,
	ID_AUTH_PREVIEW,
	ID_UPDATE_BOOK,
	ID_OPEN_WEB,
	ID_COPY_URL,
	ID_UNSELECTALL,
	ID_UPDATE_FONTS,
	ID_OPEN_BOOK,
	ID_OPEN_FILE,
	ID_SAVE_FILE,
	ID_SHOW_FILE,
	ID_COPY_LINK,

	// Master menu
	ID_MASTER_APPEND,
	ID_MASTER_MODIFY,
	ID_MASTER_REPLACE,
	ID_MASTER_PAGE,
	ID_DELETE_LINK,

	// Books menu
	ID_SHOW_COLUMNS,
	ID_DIRECTION,
	ID_ORDER_MENU,
	ID_ORDER_AUTHOR,
	ID_ORDER_RATING,
	ID_ORDER_GENRE,
	ID_ORDER_LANG,
	ID_FILTER_SET,
	ID_FILTER_USE,
	ID_FILTER_DEL,
	ID_EDIT_COMMENTS,

	// Reader menu
	ID_READER_OPTIONS,
	ID_READER_CONTENT,
	ID_READER_ZOOM_IN,
	ID_READER_ZOOM_OUT,
	ID_READER_HEADER,

	// Last submenu ID
	ID_SUBMENU_LAST,

	// Book tools
	ID_APPEND_BOOK,
	ID_INIT_SEARCH,
	ID_FOUND_NOTHING,
	ID_APPEND_FOLDER,
	ID_MODIFY_FOLDER,
	ID_DELETE_FOLDER,
	ID_UPDATE_FOLDER,
	ID_UPDATE_MASTER,
	ID_HTML_CAPTION,
	ID_HTML_COMMENT,
	ID_HTML_SUBMIT,
	ID_HTML_MODIFY,
	ID_BOOK_PAGE,
	ID_BOOK_AUTH,
	ID_BOOK_SEQN,
	ID_DOWNLOAD_BOOK,
	ID_SYSTEM_DOWNLOAD,
	ID_DELETE_DOWNLOAD,
	ID_DATABASE_INFO,
	ID_FAVORITES_DEL,
	ID_START,
	ID_PAUSE,
	ID_BOOKS_COUNT,
	ID_RATING_0,
	ID_RATING_1,
	ID_RATING_2,
	ID_RATING_3,
	ID_RATING_4,
	ID_RATING_5,
	ID_SCRIPT_RUN,
	ID_SCRIPT_LOG,
	ID_SCRIPT_ERROR,
	ID_SCRIPT_EXIT,
	ID_MODEL_CREATE,
	ID_MODEL_APPEND,
	ID_MODEL_NUMBER,
	ID_FAVORITES_ADD, // Always the last !!!
	ID_MENU_HIGHEST, // Always the last !!!
};

class MyRuLib
{
public:
	static wxString ProgramName();
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

#define COLON wxString(wxT(": "))

void FbLogMessage(const wxString &info, const wxString &text);

void FbLogError(const wxString &info, const wxString &text);

void FbLogWarning(const wxString &info, const wxString &text);

void FbMessageBox(const wxString &info, const wxString &text);

wxString GetRatingText(int index);

#endif // __FBCONST_H__
