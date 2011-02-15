#ifndef __FBFRAMESEQN_H__
#define __FBFRAMESEQN_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"
#include "FbSeqnList.h"
#include "controls/FbSearchCombo.h"

class FbFrameSeqn : public FbFrameBase
{
	public:
		FbFrameSeqn(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Series"); };
		void FindSequence(const wxString &text);
		void OpenSequence(const int sequence, const int book);
	protected:
		virtual void CreateControls();
		virtual wxMenuBar * CreateMenuBar();
		virtual wxToolBar * CreateToolBar(long style = wxTB_FLAT|wxTB_NODIVIDER|wxTB_HORZ_TEXT, wxWindowID winid = wxID_ANY, const wxString& name = wxEmptyString);
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void CreateMasterThread();
		void CreateColumns();
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
		DECLARE_EVENT_TABLE()
		DECLARE_CLASS(FbFrameSeqn)
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

#endif // __FBFRAMESEQN_H__
