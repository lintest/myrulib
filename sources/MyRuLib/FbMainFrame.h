#ifndef __FBMAINFRAME_H__
#define __FBMAINFRAME_H__

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include "controls/ProgressBar.h"
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
		wxWindow * GetActiveChild();
		void CreateControls();
		void CreateStatusBar(bool show);
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
		void UpdateMenuRefs();
		void RestoreFrameList();
		void OpenInfo(const FbMasterInfo & info, const wxString & title, wxWindowID winid);
		void OpenClss(int code, bool select = true);
		wxWindow * CreateFrame(wxWindowID id, bool select = false);
	private:
		wxEvent * m_LastEvent;
		wxTextCtrl * m_FindAuthor;
		wxTextCtrl * m_FindTitle;
		ProgressBar * m_ProgressBar;
		wxAuiManager m_FrameManager;
		wxAuiNotebook m_FrameNotebook;
		FbTreeViewCtrl * m_LogCtrl;
	private:
		wxMenu * m_MenuBook;
		wxMenu * m_MenuTree;
		wxMenu * m_MenuRead;
	private:
		void OnDisableUI(wxUpdateUIEvent & event) {
			event.Enable(false);
		}
	private:
		void OnExit(wxCommandEvent & event);
		void OnSetup(wxCommandEvent & event);
		void OnReader(wxCommandEvent & event);
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
		void OnHideLog(wxCommandEvent& event);
		void OnUpdateFolder(FbFolderEvent & event);
		void OnUpdateMaster(FbMasterEvent & event);
		void OnOpenAuth(FbOpenEvent & event);
		void OnOpenSeqn(FbOpenEvent & event);
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
		void OnStatusBar(wxCommandEvent & event);
		void OnFoundNothing(wxCommandEvent & event);
		void OnAllowNotebookDnD(wxAuiNotebookEvent& event);
		void OnPaneClose(wxAuiManagerEvent& event);
		void OnNotebookChanged(wxAuiNotebookEvent& event);
		void OnNotebookClosed(wxAuiNotebookEvent& event);
		void OnSubmenu(wxCommandEvent& event);
		void OnStatusBarUpdate(wxUpdateUIEvent  & event);
		void OnSubmenuUpdateUI(wxUpdateUIEvent & event);
		void OnIdle( wxIdleEvent & event);
		void OnNavigationKey(wxNavigationKeyEvent& event);
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbMainFrame);
		friend class FbEventLocker;
};

#endif // __FBMAINFRAME_H__
