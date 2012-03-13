#ifndef __FBPREVIEWWINDOW_H__
#define __FBPREVIEWWINDOW_H__

#include <wx/html/htmlcell.h>
#include "controls/FbHtmlWindow.h"
#include "controls/FbTreeModel.h"
#include "FbViewContext.h"
#include "FbPreviewThread.h"
#include "FbThread.h"
#include "FbMenu.h"

class FbPreviewWindow: public FbHtmlWindow
{
	public:
		FbPreviewWindow();
		FbPreviewWindow(
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxSUNKEN_BORDER
		);
		bool Create(
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxSUNKEN_BORDER
		);
		~FbPreviewWindow();
		void Reset(const FbViewContext &ctx, const FbViewItem &item);
		void Empty();
	protected:
		void OnLinkClicked(const wxHtmlLinkInfo &link);
	private:
		class ContextMenu: public FbMenu {
			public: ContextMenu(int book, wxHtmlLinkInfo * link);
		};
		void SaveFile(wxInputStream &stream, const wxString &filename);
		FbPreviewThread * m_thread;
		FbViewItem m_view;
		int m_book;
		wxHtmlLinkInfo m_link;
	private:
		void OnCellHover(wxHtmlCellEvent& event);
		void OnRightUp(wxMouseEvent& event);
		void OnInfoUpdate(wxCommandEvent& event);
		void OnCopy(wxCommandEvent& event);
		void OnSelectAll(wxCommandEvent& event);
		void OnUnselectAll(wxCommandEvent& event);
		void OnSaveFile(wxCommandEvent& event);
		void OnShowFile(wxCommandEvent& event);
		void OnCopyUrl(wxCommandEvent& event);
		DECLARE_CLASS(FbPreviewWindow)
		DECLARE_EVENT_TABLE();
};

#endif // __FBPREVIEWWINDOW_H__
