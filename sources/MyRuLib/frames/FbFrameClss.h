#ifndef __FBFRAMECLSS_H__
#define __FBFRAMECLSS_H__

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/aui/tabmdi.h>
#include <wx/listbox.h>
#include <wx/splitter.h>
#include <wx/html/htmlwin.h>
#include "FbFrameBase.h"

class FbFrameClss : public FbFrameBase
{
	public:
		static FbFrameClss * Create(wxAuiNotebook * parent, int code, bool select = false);
		FbFrameClss(wxAuiNotebook * parent, FbSQLite3ResultSet & result, bool select = false);
		int GetCode() const { return m_code; } ;
	protected:
		class MasterMenu: public wxMenu {
			public: MasterMenu(const wxString & code);
		};
		void CreateColumns();
	private:
		void CreateModel(FbSQLite3ResultSet & result);
		void ReplaceData(int old_id, int new_id, wxTreeItemId selected, const wxString &newname);
		void ShowContextMenu(const wxPoint& pos, wxTreeItemId item);
		void SelectFirstAuthor(const int book = 0);
		void AppendAttay(FbTreeModel &model, const wxArrayInt &items);
	private:
		const int m_code;
		wxSplitterWindow * m_BooksSplitter;
	private:
		void OnContextMenu(wxTreeEvent& event);
		void OnItemActivated(wxTreeEvent & event);
		void OnBooksCount(FbCountEvent & event);
		DECLARE_CLASS(FbFrameClss)
		DECLARE_EVENT_TABLE()
};

#endif // __FBFRAMECLSS_H__
