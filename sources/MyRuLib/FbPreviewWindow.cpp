#include "FbPreviewWindow.h"
#include <wx/clipbrd.h>
#include <wx/zipstrm.h>
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbFileReader.h"
#include "MyRuLibApp.h"

IMPLEMENT_CLASS(FbPreviewWindow, FbHtmlWindow)

BEGIN_EVENT_TABLE(FbPreviewWindow, FbHtmlWindow)
	EVT_RIGHT_UP(FbPreviewWindow::OnRightUp)
	EVT_HTML_CELL_HOVER(wxID_ANY, FbPreviewWindow::OnCellHover)
	EVT_COMMAND(ID_BOOK_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_COMMAND(ID_AUTH_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_MENU(ID_SAVE_FILE, FbPreviewWindow::OnSaveFile)
	EVT_MENU(ID_COPY_URL, FbPreviewWindow::OnCopyUrl)
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
		FbCollection::LoadIcon(item.GetCode());
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
	m_link = link ? *link : wxHtmlLinkInfo();

	SetFocus();
	ContextMenu menu(m_book, link);
	PopupMenu( &menu, event.GetPosition() );
}

FbPreviewWindow::ContextMenu::ContextMenu(int book, wxHtmlLinkInfo * link)
{
	if (link) {
		wxString addr = link->GetHref();
		if (addr.BeforeFirst(wxT(':')) == wxT("book")) {
			Append(ID_SAVE_FILE, _("Save file as..."));
		}
		Append(ID_COPY_URL, _("Copy link address"));
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

void FbPreviewWindow::OnCellHover(wxHtmlCellEvent& event)
{
	wxHtmlCell * cell = event.GetCell();
	wxHtmlLinkInfo * link = cell ? cell->GetLink() : NULL;
	wxString text = link ? link->GetHref() : wxString();
	event.Skip();
}

wxString FbPreviewWindow::GetName(const wxHtmlLinkInfo &link)
{
	wxString addr = link.GetHref();
	if (addr.BeforeFirst(wxT(':')) != wxT("book")) return wxEmptyString;
	return addr.AfterFirst(wxT(':'));
}

wxString FbPreviewWindow::FindFile(const wxString & name)
{
	wxString root = wxGetApp().GetLibPath();
	if (root.IsEmpty()) return name;
	if (name.IsEmpty()) return wxEmptyString;
	wxFileName filename = name;
	filename.Normalize(wxPATH_NORM_ALL, root);
	if (filename.FileExists()) return filename.GetFullPath();
	return wxEmptyString;
}

void FbPreviewWindow::OnLinkClicked(const wxHtmlLinkInfo &link)
{
	wxString name = GetName(link);
	if (name.IsEmpty()) { wxHtmlWindow::OnLinkClicked(link); return; }

	wxString file = FindFile(name);
	if (file.IsEmpty()) { wxLogWarning(_("File not found: ") + name); return; }

	FbFileReader::ShellExecute(file);
}

void FbPreviewWindow::OnSaveFile(wxCommandEvent& event)
{
	wxString name = GetName(m_link);
	if (name.IsEmpty()) return;

	wxString file = FindFile(name);
	if (file.IsEmpty()) {wxLogWarning(_("File not found: ") + name); return; }

	wxFileInputStream in(file);
	wxString targ = m_link.GetTarget();
	if (!targ.IsEmpty()) {
		bool ok = false;
		wxZipInputStream zip(in);
		while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
			if (entry->GetInternalName() == targ) {
				ok = zip.OpenEntry(*entry);
				break;
			}
		}
		if (ok) return SaveFile(zip, targ);
	}
	return SaveFile(in, name);
}

void FbPreviewWindow::OnCopyUrl(wxCommandEvent& event)
{
	wxString name = GetName(m_link);
	if (name.IsEmpty()) return;
	wxString file = FindFile(name);
	if (file.IsEmpty()) file = name;
	wxClipboardLocker locker;
	if (!locker) return;
	wxTheClipboard->SetData( new wxTextDataObject(file) );
}

void FbPreviewWindow::SaveFile(wxInputStream &stream, const wxString &filename)
{
	wxFileDialog dlg (
		this,
		_("Save file as..."),
		wxEmptyString,
		wxFileName(filename).GetFullName(),
		wxString(_("All files")) + wxT(" (*.*)|*.*"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT
	);

	if (dlg.ShowModal() == wxID_OK) {
		wxString html = * GetParser()->GetSource();
		wxFileOutputStream out(dlg.GetPath());
		out.Write(stream);
	}
}
