/***************************************************************
 * Name:	  MyRuLibMain.h
 * Purpose:   Defines Application Frame
 * Author:	Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef __FBMAINFRAME_H__
#define __FBMAINFRAME_H__

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include "ProgressBar.h"
#include "LimitedTextCtrl.h"
#include "FbFrameAuthor.h"
#include "FbLogoBitmap.h"

class FbMainFrame: public wxAuiMDIParentFrame
{
	public:
		FbMainFrame();
		virtual ~FbMainFrame();
		virtual wxString GetTitle() const;
		void SetStatus(const wxString &text);
	protected:
		virtual void SetMenuBar(wxMenuBar *pMenuBar);
	private:
		bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
		void CreateControls();
		void LoadIcon();
		wxAuiToolBar * CreateToolBar();
		void TogglePaneVisibility(const wxString &pane_name, bool show);
		void ShowPane(const wxString &pane_name);
		void FindAuthor(const wxString &text);
		void FindTitle(const wxString &title, const wxString &author);
		wxWindow * FindFrameById(const int id, bool bActivate = false);
		void OpenDatabase(const wxString &filename);
		void SetAccelerators();
	private:
		wxTextCtrl m_FindAuthor;
		wxTextCtrl m_FindTitle;
		ProgressBar m_ProgressBar;
		wxAuiToolBar * m_ToolBar;
		wxAuiManager m_FrameManager;
		LimitedTextCtrl m_LOGTextCtrl;
	private:
		void OnExit(wxCommandEvent & event);
		void OnSetup(wxCommandEvent & event);
		void OnOpenWeb(wxCommandEvent & event);
		void OnAbout(wxCommandEvent & event);
		void OnDatabaseInfo(wxCommandEvent & event);
		void OnDatabaseOpen(wxCommandEvent & event);
		void OnInfoCommand(wxCommandEvent & event);
		void OnFindAuthor(wxCommandEvent& event);
		void OnFindAuthorEnter(wxCommandEvent& event);
		void OnFindTitle(wxCommandEvent & event);
		void OnFindTitleEnter(wxCommandEvent& event);
		void OnFullScreen(wxCommandEvent& event);
		void OnFullScreenUpdate(wxUpdateUIEvent& event);
		void OnMenuNothing(wxCommandEvent& event);
		void OnMenuAuthor(wxCommandEvent& event);
		void OnMenuConfig(wxCommandEvent& event);
		void OnMenuTitle(wxCommandEvent& event);
		void OnMenuGenres(wxCommandEvent & event);
		void OnMenuFolder(wxCommandEvent & event);
		void OnMenuDownld(wxCommandEvent & event);
		void OnMenuSequen(wxCommandEvent & event);
		void OnMenuCalendar(wxCommandEvent & event);
		void OnNewZip( wxCommandEvent& event );
		void OnRegZip( wxCommandEvent& event );
		void OnFolder( wxCommandEvent& event );
		void OnProgressUpdate(wxUpdateUIEvent& event);
		void OnError(wxCommandEvent& event);
		void OnHideLog(wxCommandEvent& event);
		void OnPanelClosed(wxAuiManagerEvent& event);
		void OnNotebookPageClose(wxAuiNotebookEvent& evt);
		void OnUpdateFolder(FbFolderEvent & event);
		void OnOpenAuthor(FbOpenEvent & event);
		void OnOpenSequence(FbOpenEvent & event);
		void OnVacuum(wxCommandEvent & event);
		void OnUpdateFonts(wxCommandEvent & event);
		void OnProgress(FbProgressEvent & event);
		void OnUpdateBook(wxCommandEvent & event);
		void OnMenuRecent(wxCommandEvent & event);
		void OnRecentUpdate(wxUpdateUIEvent& event);
		void OnWindowClose(wxCommandEvent & event);
		void OnWindowCloseAll(wxCommandEvent & event);
		void OnWindowNext(wxCommandEvent & event);
		void OnWindowPrev(wxCommandEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBMAINFRAME_H__
