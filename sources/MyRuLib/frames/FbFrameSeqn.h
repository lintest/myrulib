#ifndef __FBFRAMESEQN_H__
#define __FBFRAMESEQN_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "models/FbSeqnList.h"
#include "controls/FbSearchCombo.h"

class FbSeqnViewCtrl
	: public FbMasterViewCtrl
{
private:
	void OnMasterAppend(wxCommandEvent& event);
	void OnMasterModify(wxCommandEvent& event);
	void OnMasterDelete(wxCommandEvent& event);
	void OnMasterPage(wxCommandEvent& event);
	void OnMasterPageUpdateUI(wxUpdateUIEvent & event);
	DECLARE_EVENT_TABLE()
};

class FbFrameSeqn : public FbFrameBase
{
	public:
		FbFrameSeqn(wxAuiNotebook * parent, bool select = false);
		virtual wxString GetTitle() const { return _("Series"); };
		void FindSequence(const wxString &text);
		void OpenSequence(const int sequence, const int book);
	protected:
		class MasterMenu: public wxMenu { 
			public: MasterMenu(int id);
		};
		void CreateColumns();
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void CreateMasterThread();
	private:
		wxSplitterWindow * m_BooksSplitter;
		FbSearchCombo * m_FindText;
		wxStaticText * m_FindInfo;
	private:
		wxString m_info;
		int m_SequenceCode;
	private:
		void OnBooksCount(FbCountEvent& event);
		void OnColClick(wxListEvent& event);
		void OnContextMenu(wxTreeEvent& event);
		void OnLetterClicked(wxCommandEvent& event);
		void OnCharEvent(wxKeyEvent& event);
		void OnFindEnter(wxCommandEvent& event);
		void OnMasterAppend(wxCommandEvent& event);
		void OnMasterModify(wxCommandEvent& event);
		void OnMasterDelete(wxCommandEvent& event);
		void OnAppendAuthor(wxCommandEvent& event);
		void OnAppendSequence(wxCommandEvent& event);
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
		void OnNumber(wxCommandEvent& event);
		DECLARE_CLASS(FbFrameSeqn)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMESEQN_H__
