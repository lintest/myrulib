#include "FbPreviewWindow.h"
#include "FbConst.h"
#include "FbBookData.h"
#include "FbCollection.h"

IMPLEMENT_CLASS(FbPreviewWindow, FbHtmlWindow)

BEGIN_EVENT_TABLE(FbPreviewWindow, FbHtmlWindow)
	EVT_RIGHT_UP(FbPreviewWindow::OnRightUp)
	EVT_MENU(ID_BOOK_PREVIEW, FbPreviewWindow::OnInfoUpdate)
	EVT_MENU(ID_AUTH_PREVIEW, FbPreviewWindow::OnInfoUpdate)
END_EVENT_TABLE()


FbPreviewWindow::FbPreviewWindow()
	: m_thread(new FbPreviewThread(this)), m_book(0)
{
	m_thread->Execute();
}

FbPreviewWindow::FbPreviewWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FbHtmlWindow(parent, id, pos, size, style), m_thread(new FbPreviewThread(this)), m_book(0)
{
	m_thread->Execute();
}

bool FbPreviewWindow::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	return FbHtmlWindow::Create(parent, id, pos, size, style);
}

FbPreviewWindow::~FbPreviewWindow()
{
	m_thread->Close();
	m_thread->Wait();
	delete m_thread;
}

void FbPreviewWindow::Reset(const FbViewContext &ctx, const FbViewItem &item)
{
	if (item.GetType() == FbViewItem::Book) {
		int id = item.GetCode();
		if (id) FbBookData(id).LoadIcon();
	}
	m_thread->Reset(ctx, item);
}

void FbPreviewWindow::OnInfoUpdate(wxCommandEvent& event)
{
	m_book = event.GetInt();
	SetPage(event.GetString());
}

void FbPreviewWindow::OnRightUp(wxMouseEvent& event)
{
	SetFocus();
	ContextMenu menu(m_book);
	PopupMenu( &menu, event.GetPosition() );
}

FbPreviewWindow::ContextMenu::ContextMenu(int book)
{
	Append(wxID_COPY, _("Copy") + (wxString)wxT("\tCtrl+C"), wxART_COPY);
	AppendSeparator();
	Append(wxID_SELECTALL, _("Select all") + (wxString)wxT("\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Undo selection"));
}

void FbPreviewWindow::Empty()
{
	SetPage(wxEmptyString);
	m_book = 0;
}
