#ifndef __FBFRAMEAUTH_H__
#define __FBFRAMEAUTH_H__

#include <wx/wx.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbAuthList.h"
#include "FbAlphabet.h"

enum FbAuthorListMode
{
	FB_AUTHOR_MODE_CHAR,
	FB_AUTHOR_MODE_TEXT,
	FB_AUTHOR_MODE_CODE,
};

class FbFrameAuth : public FbFrameBase
{
	public:
		FbFrameAuth(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Authors"); };
		void FindAuthor(const wxString &text);
		void ActivateAuthors();
		virtual void UpdateFonts(bool refresh);
	protected:
		virtual void CreateControls();
		virtual wxMenuBar * CreateMenuBar();
	private:
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void CreateMasterThread();
		void CreateColumns();
	private:
		wxSplitterWindow * m_BooksSplitter;
		FbAlphabetCombo * m_LetterList;
	private:
		FbAuthListInfo m_info;
		wxString letters;
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnColClick(wxListEvent& event);
		void OnAllClicked(wxCommandEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnChoiceLetter(wxCommandEvent& event);
		void OnViewAlphavet(wxCommandEvent& event);
		void OnViewAlphavetUpdateUI(wxUpdateUIEvent & event);
		void OnMasterAppend(wxCommandEvent& event);
		void OnMasterModify(wxCommandEvent& event);
		void OnMasterDelete(wxCommandEvent& event);
		void OnMasterReplace(wxCommandEvent& event);
		void OnMasterPage(wxCommandEvent& event);
		void OnMasterPageUpdateUI(wxUpdateUIEvent & event);
		void OnContextMenu(wxTreeEvent& event);
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameAuth)
	protected:
		class MasterMenu: public wxMenu
		{
			public:
				MasterMenu(int id);
		};
	private:
		class MenuBar: public FbFrameMenu
		{
			public:
				MenuBar();
		};
		class MenuMaster: public FbMenu {
			public: MenuMaster();
		};
};

#endif // __FBFRAMEAUTH_H__
