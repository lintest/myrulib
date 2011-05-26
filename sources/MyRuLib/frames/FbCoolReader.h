#ifndef __FBCOOLREADER_H__
#define __FBCOOLREADER_H__

#ifdef FB_INCLUDE_READER

#include <crgui.h>
/**
 * @short XML Document View window
 * @author Vadim Lopatin <vadim.lopatin@coolreader.org>
 * @version 0.1
 */

#define PROP_CRENGINE_FONT_SIZE     "crengine.font.size"
#define PROP_WINDOW_RECT            "window.rect"
#define PROP_WINDOW_FULLSCREEN      "window.fullscreen"
#define PROP_WINDOW_MINIMIZED       "window.minimized"
#define PROP_WINDOW_MAXIMIZED       "window.maximized"
#define PROP_WINDOW_SHOW_MENU       "window.menu.show"
#define PROP_WINDOW_ROTATE_ANGLE    "window.rotate.angle"
#define PROP_WINDOW_TOOLBAR_SIZE    "window.toolbar.size"
#define PROP_WINDOW_TOOLBAR_POSITION "window.toolbar.position"
#define PROP_WINDOW_SHOW_STATUSBAR  "window.statusbar.show"

#define PROP_PAGE_HEADER_ENABLED     "page.header.enabled"
#define PROP_PAGE_HEADER_PAGE_NUMBER "page.header.pagenumber"
#define PROP_PAGE_HEADER_PAGE_COUNT  "page.header.pagecount"
#define PROP_PAGE_HEADER_CLOCK       "page.header.clock"
#define PROP_PAGE_HEADER_BATTERY     "page.header.battery"
#define PROP_PAGE_HEADER_AUTHOR      "page.header.author"
#define PROP_PAGE_HEADER_TITLE       "page.header.title"

//#define PROP_PAGE_VIEW_MODE          "page.view.mode"
#define PROP_FONT_ANTIALIASING       "font.antialiasing.mode"
#define PROP_FONT_COLOR              "font.color.default"
#define PROP_BACKGROUND_COLOR        "background.color.default"
#define PROP_FONT_FACE               "font.face.default"

#define PROP_APP_OPEN_LAST_BOOK      "app.init.open-recent"

enum
{
	Menu_File_Quit = 100,
	Menu_File_About,
	Menu_File_Options,
    Menu_View_ZoomIn,
    Menu_View_ZoomOut,
    Menu_View_NextPage,
    Menu_View_PrevPage,
    Menu_View_NextLine,
    Menu_View_PrevLine,
    Menu_View_Text_Format,
    Menu_Link_Back,
    Menu_Link_Forward,
    Menu_Link_Next,
    Menu_Link_Prev,
    Menu_Link_Go,
    Menu_View_Begin,
    Menu_View_End,
    Menu_View_ToggleFullScreen,
    Menu_View_TogglePages,
    Menu_View_TogglePageHeader,
    Menu_View_TOC,
    Menu_View_History,
    Menu_View_Rotate,
};

enum active_mode_t {
    am_none,
    am_book,
    am_history
};

enum
{
	Window_Id_Scrollbar = 1000,
	Window_Id_View,
    Window_Id_HistList,
	Window_Id_Options,
};

class FbCoolReader
	: public wxWindow, public LVDocViewCallback
{
	public:
		FbCoolReader( wxWindow* parent, wxWindowID id, const wxPoint& p, const wxSize& sz );
		virtual ~FbCoolReader();

        void OnOptionsChange( CRPropRef oldprops, CRPropRef newprops, CRPropRef changed );

		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );
        void OnScroll( wxScrollWinEvent& event );
        void OnFileOpen( wxCommandEvent& event );
        void OnFileSave( wxCommandEvent& event );
        void OnCommand( wxCommandEvent& event );
        void OnRotate( wxCommandEvent& event );
        void OnShowOptions( wxCommandEvent& event );
        void OnShowTOC( wxCommandEvent& event );
        void OnShowHistory( wxCommandEvent& event );
        void OnUpdateUI( wxUpdateUIEvent& event );
        void OnClose( wxCloseEvent& event );
        void OnMouseWheel( wxMouseEvent& event);
        void OnInitDialog( wxInitDialogEvent& event);

        CRPropRef getProps() { return _props; }
        void SaveOptions();
        void RestoreOptions();
        void SetMenu( bool visible );

        wxBitmap getIcon16x16( const lChar16 * name );
	public:
        void ScheduleRender() { Resize(0, 0); }
        bool LoadDocument( const wxString & fname );
        void CloseDocument();
        void UpdateScrollBar();
        LVDocView * getDocView() { return _docwin->getDocView(); }
        void doCommand( LVDocCmd cmd, int param );
        void goToBookmark(ldomXPointer bm);
        wxColour getBackgroundColour();
        void SetPageHeaderFlags();
        void SetRotate( cr_rotate_angle_t angle );
        void Rotate( bool ccw = false );
        // event handlers
        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnMouseLDown( wxMouseEvent & event );
        void OnMouseRDown( wxMouseEvent & event );
        void OnMouseMotion(wxMouseEvent& event);
        void OnTimer(wxTimerEvent& event);
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // reduce flicker
        void ToggleViewMode();
        lString16 GetHistoryFileName();
        lString16 GetLastRecentFileName();
        // LVDocViewCallback override
        virtual void OnExternalLink( lString16 url, ldomNode * node );
    protected:

        void Paint();
        void Resize(int dx, int dy);
    private:
        wxCursor _normalCursor;
        wxCursor _linkCursor;

        wxTimer * _renderTimer;
        wxTimer * _clockTimer;
        wxTimer * _cursorTimer;
        bool _firstRender;
        bool _allowRender;

	protected:
        CRWxScreen _screen;
        CRGUIWindowManager _wm;
        CRDocViewWindow * _docwin;
        lString16 _appDir;
        CRPropRef _props;
	private:
		DECLARE_EVENT_TABLE()
};

int propsToPageHeaderFlags( CRPropRef props );

#endif // FB_INCLUDE_READER

#endif // __FBCOOLREADER_H__

