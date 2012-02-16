#include "FbPreviewWindow.h"
#include <wx/clipbrd.h>
#include "FbConst.h"
#include "FbBookData.h"
#include "FbBookEvent.h"
#include "FbCollection.h"

IMPLEMENT_CLASS(FbPreviewWindow, FbHtmlWindow)

BEGIN_EVENT_TABLE(FbPreviewWindow, FbHtmlWindow)
	EVT_RIGHT_UP(FbPreviewWindow::OnRightUp)
	EVT_COMMAND(ID_BOOK_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_COMMAND(ID_AUTH_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_MENU(wxID_COPY, FbPreviewWindow::OnCopy)
	EVT_MENU(wxID_SELECTALL, FbPreviewWindow::OnSelectAll)
	EVT_MENU(ID_UNSELECTALL, FbPreviewWindow::OnUnselectAll)
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
	wxDELETE(m_thread);
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
	wxPoint p = event.GetPosition();
	wxHtmlCell * cell = m_Cell ? m_Cell->FindCellByPos(p.x, p.y) : NULL;
	wxHtmlLinkInfo * link = cell ? cell->GetLink() : NULL;

	SetFocus();
	ContextMenu menu(m_book, link);
	PopupMenu( &menu, event.GetPosition() );
}

FbPreviewWindow::ContextMenu::ContextMenu(int book, wxHtmlLinkInfo * link)
{
	if (link) { 
		Append(wxID_ANY, wxT("Copy link address"));
		AppendSeparator();
	}

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

void FbPreviewWindow::OnCopy(wxCommandEvent& event)
{
	wxString text = SelectionToText();
	wxClipboardLocker locker;
	if (!locker) return;
	wxTheClipboard->SetData( new wxTextDataObject(text) );
}

void FbPreviewWindow::OnSelectAll(wxCommandEvent& event)
{
	SelectAll();
}

void FbPreviewWindow::OnUnselectAll(wxCommandEvent& event)
{
	UnselectALL();
	Refresh();
}

void FbPreviewWindow::OnLinkClicked(const wxHtmlLinkInfo &link)
{
	wxString addr = link.GetHref();
	if (addr.BeforeFirst(wxT(':')) == wxT("book")) {
		wxLogWarning(addr);
		return;
	}
	wxHtmlWindow::OnLinkClicked(link);
}

