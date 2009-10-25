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
private:
	bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
	void CreateControls();
	wxAuiToolBar * CreateToolBar();
	void TogglePaneVisibility(const wxString &pane_name, bool show);
	void ShowPane(const wxString &pane_name);
	void FindAuthor(const wxString &text);
	void FindTitle(const wxString &text);
	wxWindow * FindFrameById(const int id, bool bActivate = false);
private:
	wxTextCtrl m_FindAuthor;
	wxTextCtrl m_FindTitle;
	ProgressBar m_ProgressBar;
	wxString m_StatusText;
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
	void OnMenuNothing(wxCommandEvent& event);
	void OnMenuAuthor(wxCommandEvent& event);
	void OnMenuConfig(wxCommandEvent& event);
	void OnMenuTitle(wxCommandEvent& event);
	void OnMenuGenres(wxCommandEvent & event);
	void OnMenuFavour(wxCommandEvent & event);
	void OnNewZip( wxCommandEvent& event );
	void OnRegZip( wxCommandEvent& event );
	void OnFolder( wxCommandEvent& event );
	void OnProgressStart(wxUpdateUIEvent& event);
	void OnProgressUpdate(wxUpdateUIEvent& event);
	void OnProgressFinish(wxUpdateUIEvent& event);
	void OnError(wxCommandEvent& event);
	void OnHideLog(wxCommandEvent& event);
	void OnPanelClosed(wxAuiManagerEvent& event);
	void OnNotebookPageClose(wxAuiNotebookEvent& evt);
	void OnUpdateFolder(FbFolderEvent & event);
	void OnOpenAuthor(FbOpenEvent & event);
	void OnVacuum(wxCommandEvent & event);
	void OnProgress(FbProgressEvent & event);
	void OnUpdateAll(wxCommandEvent & event);
	DECLARE_EVENT_TABLE()
};

#endif // __FBMAINFRAME_H__
