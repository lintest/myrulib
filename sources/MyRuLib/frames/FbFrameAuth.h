#ifndef __FBFRAMEAUTH_H__
#define __FBFRAMEAUTH_H__

#include <wx/wx.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "models/FbAuthList.h"
#include "FbAlphabet.h"

class FbAuthViewCtrl
	: public FbMasterViewCtrl
{
private:
	void OnMasterReplace(wxCommandEvent& event);
	void OnMasterAppend(wxCommandEvent& event);
	void OnMasterModify(wxCommandEvent& event);
	void OnMasterDelete(wxCommandEvent& event);
	void OnMasterPage(wxCommandEvent& event);
	void OnMasterPageUpdateUI(wxUpdateUIEvent & event);
	DECLARE_EVENT_TABLE()
};

class FbFrameAuth : public FbFrameBase
{
	public:
		FbFrameAuth(wxAuiNotebook * parent, bool select = false);
		void FindAuthor(const wxString &text);
		void ActivateAuthors();
		virtual void UpdateFonts(bool refresh);
		virtual void UpdateMaster();
	protected:
		virtual wxString GetCountSQL();
	private:
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void CreateMasterThread();
		void CreateColumns();
	private:
		FbAlphabetCombo * m_LetterList;
		FbAuthListInfo m_info;
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnColClick(wxListEvent& event);
		void OnAllClicked(wxCommandEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnChoiceLetter(wxCommandEvent& event);
		void OnViewAlphavet(wxCommandEvent& event);
		void OnViewAlphavetUpdateUI(wxUpdateUIEvent & event);
		void OnContextMenu(wxTreeEvent& event);
		void OnNumber(wxCommandEvent& event);
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
};

#endif // __FBFRAMEAUTH_H__
