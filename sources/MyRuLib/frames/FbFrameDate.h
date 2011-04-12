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
	protected:
		virtual void CreateControls();
		void CreateColumns();
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void SelectFirstAuthor(const int book = 0);
		void AppendAttay(FbTreeModel &model, const wxArrayInt &items);
	private:
		wxSplitterWindow * m_BooksSplitter;
		wxTextCtrl * m_FindText;
		wxStaticText * m_FindInfo;
		wxString m_SequenceText;
		int m_SequenceCode;
	private:
		void OnBooksCount(FbCountEvent & event);
		void OnModel(FbModelEvent & event);
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEDATE_H__
