#include "FbPreviewWindow.h"

IMPLEMENT_CLASS(FbPreviewWindow, FbHtmlWindow)

FbPreviewWindow::FbPreviewWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FbHtmlWindow(parent, id, pos, size, style), m_thread(NULL)
{
}

bool FbPreviewWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	return FbHtmlWindow::Create(parent, id, pos, size, style);
}
