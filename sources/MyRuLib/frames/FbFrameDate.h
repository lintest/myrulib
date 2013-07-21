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
		FbFrameDate(wxAuiNotebook * parent, bool select = false);
	protected:
		void CreateColumns();
		virtual wxString GetCountSQL();
	private:
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void SelectFirstAuthor(const int book = 0);
		void AppendAttay(FbTreeModel &model, const wxArrayInt &items);
	private:
		wxSplitterWindow * m_BooksSplitter;
	private:
		void OnBooksCount(FbCountEvent & event);
		void OnModel(FbModelEvent & event);
		void OnNumber(wxCommandEvent& event);
		DECLARE_CLASS(FbFrameDate)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMEDATE_H__
