#ifndef __FBPREVIEWWINDOW_H__
#define __FBPREVIEWWINDOW_H__

#include "FbHtmlWindow.h"
#include "FbTreeModel.h"
#include "FbThread.h"

class FbPreviewWindow: public FbHtmlWindow
{
	public:
		FbPreviewWindow() {};
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
		void Reset(const FbModelItem &item);
	private:
		FbThread * m_thread;
		DECLARE_CLASS(FbPreviewWindow)
};

#endif // __FBPREVIEWWINDOW_H__
