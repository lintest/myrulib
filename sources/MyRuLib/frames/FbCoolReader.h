#ifndef __FBCOOLREADER_H__
#define __FBCOOLREADER_H__

#ifdef FB_INCLUDE_READER

#include <crgui.h>
#include <wx/dialog.h>
#include <wx/splitter.h>
#include <wx/fdrepdlg.h>
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
	Menu_View_Text,
	Menu_Link_Back,
	Menu_Link_Forward,
	Menu_Link_Next,
	Menu_Link_Prev,
	Menu_Link_Go,
	Menu_View_Begin,
	Menu_View_End,
	Menu_View_Scroll,
	Menu_View_Header,
	Menu_View_TOC,
	Menu_View_History,
	Menu_View_Rotate,
};

enum active_mode_t {
	am_none,
	am_book,
	am_history
};

class FbCoolReader: public wxWindow, public LVDocViewCallback
{
	public:
		class ContentDlg: public wxDialog {
			public:
				static bool Execute( LVDocView * view );
				ContentDlg( wxWindow* parent, const wxString& title = wxEmptyString );
			private:
				ContentDlg * Assign( LVDocView * view );
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
	protected:
		class FindReplaceData: public wxFindReplaceData {
			public:
				int m_position;
		};
	public:
		static bool InitCREngine();
		static void GetFonts(wxArrayString & fonts);
		static FbCoolReader * Open(int book, const wxString &filename);

		FbCoolReader(int book);
		virtual ~FbCoolReader();
		bool Create(wxAuiNotebook * parent);

		void Setup(bool refresh);

		void OnOptionsChange( CRPropRef oldprops, CRPropRef newprops, CRPropRef changed );

		void DoEvent(wxEvent& event) {
			GetEventHashTable().HandleEvent(event, this);
		}

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
		void OnShowHeader( wxCommandEvent& event );
		void OnShowHeaderUI( wxUpdateUIEvent& event );
		void OnCopy( wxCommandEvent& event );
		void OnFind( wxCommandEvent& event );
		void OnFindFirst( wxFindDialogEvent& event );
		void OnFindNext( wxFindDialogEvent& event );
		void OnFindClose( wxFindDialogEvent& event );
		void OnEditBook( wxCommandEvent & event );
	private:
		void SetupPageHeader();
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
		void OnMouseLDClick( wxMouseEvent & event );
		void OnMouseMotion(wxMouseEvent& event);
		void OnIdle( wxIdleEvent &event );
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { ;; } // reduce flicker
		void ToggleViewMode();
		virtual void OnExternalLink( lString16 url, ldomNode * node );

	private:
		void OnCopuUpdateUI(wxUpdateUIEvent & event) {
			event.Enable(!m_sel_text.IsEmpty());
		}
		void OnEnableUI(wxUpdateUIEvent & event) {
			event.Enable(true);
		}
		void OnDisableUI(wxUpdateUIEvent & event) {
			event.Enable(false);
		}
	protected:
		void Repaint() { m_dirty = true; }
	private:
		int m_book;
		bool m_dirty;
		FindReplaceData * m_findData;
		wxFindReplaceDialog * m_findDlg;

		ldomXPointer m_sel_pos;
		wxString m_sel_text;

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

