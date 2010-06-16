#ifndef __FBFRAMEDATE_H__
#define __FBFRAMEDATE_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"

class FbFrameDate : public FbFrameBase
{
	public:
		FbFrameDate(wxAuiMDIParentFrame * parent);
		virtual wxString GetTitle() const { return _("Calendar"); };
		void ActivateAuthors();
		void Localize(bool bUpdateMenu);
	protected:
		virtual void CreateControls();
		virtual void UpdateBooklist();
		void CreateColumns();
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void SelectFirstAuthor(const int book = 0);
		BookTreeItemData * GetSelectedBook();
		void AppendAttay(FbTreeModel &model, const wxArrayInt &items);
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxTextCtrl * m_FindText;
		wxStaticText * m_FindInfo;
		wxString m_SequenceText;
		int m_SequenceCode;
	private:
		void OnMasterSelected(wxTreeEvent & event);
		void OnBooksCount(wxCommandEvent& event);
		void OnModel( FbArrayEvent& event );
		void OnArray( FbArrayEvent& event );
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEDATE_H__
