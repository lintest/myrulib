#ifndef __FBPREVIEWWINDOW_H__
#define __FBPREVIEWWINDOW_H__

#include "controls/FbHtmlWindow.h"
#include "controls/FbTreeModel.h"
#include "FbViewContext.h"
#include "FbPreviewThread.h"
#include "FbThread.h"

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
	private:
		FbPreviewThread * m_thread;
		FbViewItem m_view;
	private:
		void OnInfoUpdate(wxCommandEvent& event);
		DECLARE_CLASS(FbPreviewWindow)
		DECLARE_EVENT_TABLE();
};

#endif // __FBPREVIEWWINDOW_H__
