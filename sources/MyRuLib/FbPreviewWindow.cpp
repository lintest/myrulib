#include "FbPreviewWindow.h"
#include <wx/clipbrd.h>
#include <wx/zipstrm.h>
#include "FbConst.h"
#include "FbBookEvent.h"
#include "FbCollection.h"
#include "FbFileReader.h"
#include "MyRuLibApp.h"

static wxString GetLinkName(const wxHtmlLinkInfo &link)
{
	wxString addr = link.GetHref();
	if (addr.BeforeFirst(wxT(':')) != wxT("book")) return wxEmptyString;
	return addr.AfterFirst(wxT(':'));
}

static wxString FindLinkFile(const wxString & name)
{
	if (name.IsEmpty()) return name;
	wxString root = wxGetApp().GetLibPath();
	if (root.IsEmpty()) return name;
	wxFileName filename = name;
	filename.Normalize(wxPATH_NORM_ALL, root);
	if (filename.FileExists()) return filename.GetFullPath();
	return wxEmptyString;
}

static wxString GetLinkFile(const wxHtmlLinkInfo &link)
{
	const wxString name = GetLinkName(link);
	const wxString file = FindLinkFile(name);
	if (file.IsEmpty()) wxLogWarning(_("File not found: ") + name); 
	return file;
}

IMPLEMENT_CLASS(FbPreviewWindow, FbHtmlWindow)

BEGIN_EVENT_TABLE(FbPreviewWindow, FbHtmlWindow)
	EVT_RIGHT_UP(FbPreviewWindow::OnRightUp)
	EVT_HTML_CELL_HOVER(wxID_ANY, FbPreviewWindow::OnCellHover)
	EVT_COMMAND(ID_BOOK_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_COMMAND(ID_AUTH_PREVIEW, fbEVT_BOOK_ACTION, FbPreviewWindow::OnInfoUpdate)
	EVT_MENU(ID_SAVE_FILE, FbPreviewWindow::OnSaveFile)
	EVT_MENU(ID_SHOW_FILE, FbPreviewWindow::OnShowFile)
	EVT_MENU(ID_COPY_LINK, FbPreviewWindow::OnCopyUrl)
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

#ifdef __WXMSW__

#include <shellapi.h>
#include <shlobj.h>

static bool ShellContextMenu(wxWindow * window, const wxString &filename)
{
	if (filename.IsEmpty()) return false;

	HWND hwnd = (HWND) window->GetHWND();

	HMODULE hModule = LoadLibrary(wxT("shell32.dll"));
	if ( !hModule ) return false;

	typedef HRESULT (WINAPI * LPSHParse) (PCWSTR, IBindCtx*, LPITEMIDLIST*, SFGAOF, SFGAOF*);
	LPSHParse pProcParse = (LPSHParse) GetProcAddress(hModule, "SHParseDisplayName");
	if ( !pProcParse ) return false;

	LPITEMIDLIST target;
	if (FAILED(pProcParse((LPWSTR)filename.wc_str(), NULL, &target, 0, NULL))) return false;

	typedef HRESULT (WINAPI * LPSHBind) (LPCITEMIDLIST, REFIID, void**, LPCITEMIDLIST*);
	LPSHBind pProcBind = (LPSHBind) GetProcAddress(hModule, "SHBindToParent");
	if ( !pProcBind ) return false;

	LPSHELLFOLDER psf;
	LPCITEMIDLIST pidl;
	if (FAILED(pProcBind(target, IID_IShellFolder, (void**)&psf, &pidl))) return false;

	LPCONTEXTMENU cm;
	if (FAILED(psf->GetUIObjectOf(0, 1, &pidl, IID_IContextMenu, NULL, (void**)&cm))) return false;

	HMENU hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MFT_STRING, 0x7001, _("Save file as..."));
	AppendMenu(hMenu, MFT_STRING, 0x7002, _("Show in folder"));
	AppendMenu(hMenu, MFT_STRING, 0x7003, _("Copy link address"));
	AppendMenu(hMenu, MFT_SEPARATOR, 0, NULL);

	cm->QueryContextMenu(hMenu, 4, 1, 0x6FFF, CMF_EXPLORE);

	POINT pt;
	GetCursorPos(&pt);
	UINT uFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD;
	int cmd = TrackPopupMenu(hMenu, uFlags, pt.x, pt.y, 0, hwnd, 0);

	if (cmd && cmd <= 0x6FFF) {
		CMINVOKECOMMANDINFO ci;
		ZeroMemory(&ci, sizeof(ci));
		ci.cbSize = sizeof(CMINVOKECOMMANDINFO);
		ci.lpVerb = MAKEINTRESOURCEA(cmd - 1);
		ci.nShow = SW_SHOWNORMAL;
		cm->InvokeCommand(&ci);
	} else {
		wxWindowID id = 0;
		switch (cmd) {
			case 0x7001: id = ID_SAVE_FILE; break;
			case 0x7002: id = ID_SHOW_FILE; break;
			case 0x7003: id = ID_COPY_LINK; break;
		}
		if (id) FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, id).Post(window);
	}

	DestroyMenu(hMenu);
	return true;
}

#else

static bool ShellContextMenu(wxWindow * window, const wxString &filename)
{
	return false;
}

#endif // __WXMSW__

void FbPreviewWindow::OnRightUp(wxMouseEvent& event)
{
	wxPoint p = CalcUnscrolledPosition(event.GetPosition());
	wxHtmlCell * cell = m_Cell ? m_Cell->FindCellByPos(p.x, p.y) : NULL;
	wxHtmlLinkInfo * link = cell ? cell->GetLink() : NULL;
	m_link = link ? *link : wxHtmlLinkInfo();

	SetFocus();

	if (link) {
		wxString addr = link->GetHref();
		if (addr.BeforeFirst(wxT(':')) == wxT("book")) {
			const wxString name = GetLinkName(*link);
			const wxString file = FindLinkFile(name);
			if (ShellContextMenu(this, file)) return;
		}
	}

	ContextMenu menu(m_book, link);
	PopupMenu( &menu, event.GetPosition() );
}

FbPreviewWindow::ContextMenu::ContextMenu(int book, wxHtmlLinkInfo * link)
{
	if (link) {
		wxString addr = link->GetHref();
		if (addr.BeforeFirst(wxT(':')) == wxT("book")) {
			Append(ID_SAVE_FILE, _("Save file as..."));
			#ifdef __WXMSW__
			Append(ID_SHOW_FILE, _("Show in folder"));
			#endif // __WXMSW__
		}
		Append(ID_COPY_LINK, _("Copy link address"));
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

void FbPreviewWindow::OnLinkClicked(const wxHtmlLinkInfo &link)
{
	wxString filename = GetLinkFile(link);
	if (filename.IsEmpty()) { wxHtmlWindow::OnLinkClicked(link); return; }
	FbFileReader::ShellExecute(filename, link.GetTarget());
}

void FbPreviewWindow::OnSaveFile(wxCommandEvent& event)
{
	wxString filename = GetLinkFile(m_link);
	if (filename.IsEmpty()) return;

	wxFileInputStream in(filename);
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
	return SaveFile(in, filename);
}

void FbPreviewWindow::OnCopyUrl(wxCommandEvent& event)
{
	wxString name = GetLinkName(m_link);
	if (name.IsEmpty()) return;

	wxString file = FindLinkFile(name);
	if (file.IsEmpty()) file = name;

	wxFileName filename = file;
	if (!m_link.GetTarget().IsEmpty()) {
		filename = m_link.GetTarget();
		filename.MakeAbsolute(file);
	}
	file = filename.GetFullPath();

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

#ifdef __WXMSW__

#include <shlobj.h>

void FbPreviewWindow::OnShowFile(wxCommandEvent& event)
{
	wxString filename = GetLinkFile(m_link);
	if (filename.IsEmpty()) return;

	HMODULE hModule = LoadLibrary(wxT("shell32.dll"));
	if ( !hModule ) return;

	typedef HRESULT (WINAPI * LPSHParse) (PCWSTR, IBindCtx*, LPITEMIDLIST*, SFGAOF, SFGAOF*);
	LPSHParse pProcParse = (LPSHParse) GetProcAddress(hModule, "SHParseDisplayName");
	if ( !pProcParse ) return;

	typedef HRESULT (WINAPI * LPSHOpen) (LPCITEMIDLIST, UINT, LPCITEMIDLIST*, DWORD);
	LPSHOpen pProcOpen = (LPSHOpen) GetProcAddress(hModule, "SHOpenFolderAndSelectItems");
	if ( !pProcOpen ) return;

	LPITEMIDLIST  target;
	if (SUCCEEDED(pProcParse((LPWSTR)filename.wc_str(), NULL, &target, 0, NULL))) {
		pProcOpen(target, 0, NULL, NULL);
	}
}

#else

void FbPreviewWindow::OnShowFile(wxCommandEvent& event)
{
}

#endif // __WXMSW__
