#ifndef __FBMAINFRAME_H__
#define __FBMAINFRAME_H__

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include "controls/ProgressBar.h"
#include "controls/LimitedTextCtrl.h"
#include "FbLogoBitmap.h"
#include "FbBookEvent.h"
#include "FbMasterInfo.h"
#include "FbWindow.h"

class FbMainFrame : public wxFrame
{
	public:
		FbMainFrame();
		virtual ~FbMainFrame();
		virtual wxString GetTitle() const;
		void Localize(int language);
		wxAuiNotebook * GetNotebook() { return &m_FrameNotebook; }
	protected:
		virtual bool ProcessEvent(wxEvent& event);
	private:
		bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
		wxMenuBar * CreateMenuBar(wxWindow * child = NULL);
		wxWindow * GetActiveChild();
		void CreateControls();
		void LoadIcon();
		wxToolBar * CreateToolBar();
		wxAuiPaneInfo * FindLog();
		void ShowLog(bool forced = false);
		void FindAuthor(const wxString &text);
		void FindTitle(const wxString &title, const wxString &author);
		wxWindow * FindFrameById(const int id, bool bActivate = false);
		void OpenDatabase(const wxString &filename);
		void SetAccelerators();
		void SetTabArt(int id);
		void SaveFrameList();
		void RestoreFrameList();
		void OpenInfo(const FbMasterInfo & info, const wxString & text);
		wxWindow * CreateFrame(wxWindowID id, bool select = false);
	private:
		wxEvent * m_LastEvent;
		wxTextCtrl * m_FindAuthor;
		wxTextCtrl * m_FindTitle;
		ProgressBar m_ProgressBar;
		wxAuiManager m_FrameManager;
		wxAuiNotebook m_FrameNotebook;
		LimitedTextCtrl m_LogTextCtrl;
		wxToolBar * m_toolbar;
	private:
		void OnExit(wxCommandEvent & event);
		void OnSetup(wxCommandEvent & event);
		void OnOpenWeb(wxCommandEvent & event);
		void OnAbout(wxCommandEvent & event);
		void OnDatabaseInfo(wxCommandEvent & event);
		void OnDatabaseOpen(wxCommandEvent & event);
		void OnDatabaseGenres(wxCommandEvent & event);
		void OnInfoCommand(wxCommandEvent & event);
		void OnFindAuthor(wxCommandEvent& event);
		void OnFindAuthorEnter(wxCommandEvent& event);
		void OnFindTitle(wxCommandEvent & event);
		void OnFindTitleEnter(wxCommandEvent& event);
		void OnFullScreen(wxCommandEvent& event);
		void OnFullScreenUpdate(wxUpdateUIEvent& event);
		void OnMenuFrame(wxCommandEvent& event);
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
		void OnUpdateFolder(FbFolderEvent & event);
		void OnUpdateMaster(FbMasterEvent & event);
		void OnOpenAuthor(FbOpenEvent & event);
		void OnOpenSequence(FbOpenEvent & event);
		void OnTabArt(wxCommandEvent & event);
		void OnTabArtUpdate(wxUpdateUIEvent& event);
		void OnHideLogUpdate(wxUpdateUIEvent& event);
		void OnVacuum(wxCommandEvent & event);
		void OnUpdate(wxCommandEvent & event);
		void OnUpdateUpdate(wxUpdateUIEvent& event);
		void OnUpdateFonts(wxCommandEvent & event);
		void OnProgress(FbProgressEvent & event);
		void OnUpdateBook(wxCommandEvent & event);
		void OnMenuRecent(wxCommandEvent & event);
		void OnRecentUpdate(wxUpdateUIEvent& event);
		void OnWindowClose(wxCommandEvent & event);
		void OnWindowCloseAll(wxCommandEvent & event);
		void OnWindowNext(wxCommandEvent & event);
		void OnWindowPrev(wxCommandEvent & event);
		void OnAllowNotebookDnD(wxAuiNotebookEvent& event);
		void OnNotebookChanged(wxAuiNotebookEvent& event);
		void OnNotebookClosed(wxAuiNotebookEvent& event);
		void OnSubmenu(wxCommandEvent& event);
		void OnSubmenuUpdateUI(wxUpdateUIEvent & event);
		void OnIdle( wxIdleEvent & event);
		DECLARE_EVENT_TABLE()
		friend class FbEventLocker;
};

#endif // __FBMAINFRAME_H__
