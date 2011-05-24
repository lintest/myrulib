
#ifndef _CR3VIEW_H_
#define _CR3VIEW_H_

#define CR_WX_SUPPORT

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

class cr3view 
	: public wxPanel, public LVDocViewCallback
{
    public:
        cr3view(CRPropRef props, lString16 exeDirPath );
        virtual ~cr3view();
        void ScheduleRender() { Resize(0, 0); }
        bool LoadDocument( const wxString & fname );
        void CloseDocument();
        void SetScrollBar( wxScrollBar * sb ) { _scrollbar = sb; }
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
        void OnMouseWheel(wxMouseEvent& event);
        void OnScroll(wxScrollEvent& event);
        void OnCommand( wxCommandEvent& event );
        void OnSetFocus( wxFocusEvent& event );
        void OnMouseLDown( wxMouseEvent & event );
        void OnMouseRDown( wxMouseEvent & event );
        void OnMouseMotion(wxMouseEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnInitDialog(wxInitDialogEvent& event);
        void ToggleViewMode();
		void SetFullScreenState(bool fullscreenState) { _isFullscreen = fullscreenState; }
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
        //LVDocView * _docview;
        wxScrollBar * _scrollbar;

        wxTimer * _renderTimer;
        wxTimer * _clockTimer;
        wxTimer * _cursorTimer;
        bool _firstRender;
        bool _allowRender;
        CRPropRef _props;

        bool _isFullscreen;

        CRWxScreen _screen;
        CRGUIWindowManager _wm;
        CRDocViewWindow * _docwin;

        DECLARE_EVENT_TABLE()
};

int propsToPageHeaderFlags( CRPropRef props );

#endif // _CR3VIEW_H_
