#ifndef __FBCOOLREADER_H__
#define __FBCOOLREADER_H__

#ifdef FB_INCLUDE_READER

#include <crgui.h>
#include <wx/dialog.h>
#include <wx/splitter.h>
#include <wx/aui/tabmdi.h>
#include "FbMenu.h"
#include "controls/FbTreeView.h"

/**
 * @short XML Document View window
 * @author Vadim Lopatin <vadim.lopatin@coolreader.org>
 * @version 0.1
 */

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

class FbCoolReader: public wxWindow, public LVDocViewCallback
{
	public: 
		class ContentDlg: public wxDialog {
			public: 
				ContentDlg( wxWindow* parent, const wxString& title = wxEmptyString );
				void Assign( LVDocView * docView );
		        LVTocItem * GetSelection();
			private:
				FbTreeViewCtrl m_treeview;
			private:
				void OnActivated( wxTreeEvent & event );
				DECLARE_EVENT_TABLE()
		};
		class MenuBook: public FbMenu { 
			public: 
				MenuBook(); 
		};
	public:
		static bool InitCREngine();
		static void GetFonts(wxArrayString & fonts);
		static FbCoolReader * Open(wxAuiNotebook * parent, const wxString &filename, bool select = false);
		FbCoolReader(wxAuiNotebook * parent);
		virtual ~FbCoolReader();
		
		void Setup(bool refresh);

		void OnOptionsChange( CRPropRef oldprops, CRPropRef newprops, CRPropRef changed );

		void OnAbout( wxCommandEvent& event );
		void OnScroll( wxScrollWinEvent& event );
		void OnFileOpen( wxCommandEvent& event );
		void OnFileSave( wxCommandEvent& event );
		void OnCommand( wxCommandEvent& event );
		void OnRotate( wxCommandEvent& event );
		void OnShowHistory( wxCommandEvent& event );
		void OnShowContent( wxCommandEvent& event );
		void OnUpdateUI( wxUpdateUIEvent& event );
		void OnMouseWheel( wxMouseEvent& event);
		void OnInitDialog( wxInitDialogEvent& event);
	public:
		void ScheduleRender() { Resize(0, 0); }
		bool LoadDocument( const wxString & fname );
		void UpdateScrollBar();
		LVDocView * GetDocView() { return _docwin->getDocView(); }
		void doCommand( LVDocCmd cmd, int param );
		void goToBookmark(ldomXPointer bm);
		wxColour getBackgroundColour();
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
		virtual void OnExternalLink( lString16 url, ldomNode * node );
	protected:
		void SetBatteryIcons();
		void Paint();
		void Resize(int dx, int dy);
	private:
		wxTimer _renderTimer;
		wxTimer _cursorTimer;
		wxTimer _clockTimer;

		wxCursor _normalCursor;
		wxCursor _linkCursor;

		bool _firstRender;
		bool _allowRender;

	protected:
		CRWxScreen _screen;
		CRGUIWindowManager _wm;
		CRDocViewWindow * _docwin;
		lString16 _appDir;
	private:
		DECLARE_CLASS(FbCoolReader)
		DECLARE_EVENT_TABLE()
};

#endif // FB_INCLUDE_READER

#endif // __FBCOOLREADER_H__

