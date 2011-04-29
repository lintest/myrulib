#include "FbMainFrame.h"
#include <wx/splitter.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>
#include <wx/artprov.h>
#include <wx/tokenzr.h>
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbParamsDlg.h"
#include "FbImportThread.h"
#include "FbDataOpenDlg.h"
#include "frames/FbFrameAuth.h"
#include "frames/FbFrameFind.h"
#include "frames/FbFrameFldr.h"
#include "frames/FbFrameGenr.h"
#include "frames/FbFrameInfo.h"
#include "frames/FbFrameDate.h"
#include "frames/FbFrameDown.h"
#include "frames/FbFrameSeqn.h"
#include "FbMainMenu.h"
#include "FbConfigDlg.h"
#include "FbDownloader.h"
#include "FbGenreThread.h"
#include "FbUpdateThread.h"
#include "FbMasterTypes.h"
#include "FbAboutDlg.h"
#include "controls/FbNotebook.h"
#include "FbDateTime.h"
#include "FbLocale.h"

BEGIN_EVENT_TABLE(FbMainFrame, FbAuiMDIParentFrame)
	EVT_TOOL(wxID_NEW, FbMainFrame::OnNewZip)
	EVT_MENU(wxID_OPEN, FbMainFrame::OnFolder)
	EVT_MENU(wxID_EXIT, FbMainFrame::OnExit)
	EVT_MENU(ID_MENU_SEARCH, FbMainFrame::OnMenuTitle)
	EVT_MENU(ID_FRAME_AUTH, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_GENR, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_FLDR, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_DOWN, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_SEQN, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_DATE, FbMainFrame::OnMenuFrame)
	EVT_MENU(ID_FRAME_ARCH, FbMainFrame::OnMenuNothing)
	EVT_MENU(ID_MENU_DB_INFO, FbMainFrame::OnDatabaseInfo)
	EVT_MENU(ID_MENU_DB_OPEN, FbMainFrame::OnDatabaseOpen)
	EVT_MENU(ID_MENU_GENLIST, FbMainFrame::OnDatabaseGenres)
	EVT_MENU(ID_MENU_VACUUM, FbMainFrame::OnVacuum)
	EVT_MENU(ID_MENU_UPDATE, FbMainFrame::OnUpdate)
	EVT_MENU(ID_MENU_CONFIG, FbMainFrame::OnMenuConfig)
	EVT_MENU(wxID_PREFERENCES, FbMainFrame::OnSetup)
	EVT_MENU(ID_OPEN_WEB, FbMainFrame::OnOpenWeb)
	EVT_MENU(wxID_ABOUT, FbMainFrame::OnAbout)

	EVT_MENU(ID_AUTHOR_BTN, FbMainFrame::OnFindAuthor)
	EVT_TEXT_ENTER(ID_AUTHOR_TXT, FbMainFrame::OnFindAuthorEnter)
	EVT_MENU(ID_TITLE_BTN, FbMainFrame::OnFindTitle)
	EVT_TEXT_ENTER(ID_TITLE_TXT, FbMainFrame::OnFindTitleEnter)

	EVT_MENU(wxID_FILE1, FbMainFrame::OnMenuRecent)
	EVT_MENU(wxID_FILE2, FbMainFrame::OnMenuRecent)
	EVT_MENU(wxID_FILE3, FbMainFrame::OnMenuRecent)
	EVT_MENU(wxID_FILE4, FbMainFrame::OnMenuRecent)
	EVT_MENU(wxID_FILE5, FbMainFrame::OnMenuRecent)

	EVT_UPDATE_UI(wxID_FILE, FbMainFrame::OnRecentUpdate)
	EVT_UPDATE_UI(ID_PROGRESS_UPDATE, FbMainFrame::OnProgressUpdate)

	EVT_MENU(ID_ERROR, FbMainFrame::OnError)
	EVT_MENU(ID_LOG_TEXTCTRL, FbMainFrame::OnHideLog)
	EVT_MENU(ID_UPDATE_FONTS, FbMainFrame::OnUpdateFonts)
	EVT_MENU(ID_FULLSCREEN, FbMainFrame::OnFullScreen)
	EVT_MENU(ID_ART_DEFAULT, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_COMPACT, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_STANDART, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_TOOLBAR, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_BUTTONS, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_VSTUDIO, FbMainFrame::OnTabArt)
	EVT_MENU(ID_ART_MOZILLA, FbMainFrame::OnTabArt)
	EVT_UPDATE_UI(ID_MENU_UPDATE, FbMainFrame::OnUpdateUpdate)
	EVT_UPDATE_UI(ID_ART_DEFAULT, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_COMPACT, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_STANDART, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_TOOLBAR, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_BUTTONS, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_VSTUDIO, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_ART_MOZILLA, FbMainFrame::OnTabArtUpdate)
	EVT_UPDATE_UI(ID_FULLSCREEN,  FbMainFrame::OnFullScreenUpdate)
	EVT_UPDATE_UI(ID_LOG_TEXTCTRL, FbMainFrame::OnHideLogUpdate)

	EVT_MENU(wxID_CLOSE, FbMainFrame::OnWindowClose)
	EVT_MENU(wxID_CLOSE_ALL, FbMainFrame::OnWindowCloseAll)
	EVT_MENU(ID_WINDOW_NEXT, FbMainFrame::OnWindowNext)
	EVT_MENU(ID_WINDOW_PREV, FbMainFrame::OnWindowPrev)

	EVT_FB_OPEN(ID_BOOK_AUTHOR, FbMainFrame::OnOpenAuthor)
	EVT_FB_OPEN(ID_BOOK_SEQUENCE, FbMainFrame::OnOpenSequence)
	EVT_FB_FOLDER(ID_UPDATE_FOLDER, FbMainFrame::OnUpdateFolder)
	EVT_FB_MASTER(ID_UPDATE_MASTER, FbMainFrame::OnUpdateMaster)
	EVT_FB_PROGRESS(ID_PROGRESS_UPDATE, FbMainFrame::OnProgress)
	EVT_COMMAND(ID_DATABASE_INFO, fbEVT_BOOK_ACTION, FbMainFrame::OnInfoCommand)
	EVT_COMMAND(ID_UPDATE_BOOK, fbEVT_BOOK_ACTION, FbMainFrame::OnUpdateBook)
END_EVENT_TABLE()

wxString FbMainFrame::GetTitle() const
{
	return FbParams::GetStr(DB_LIBRARY_TITLE) + wxT(" - ") + MyRuLib::ProgramName();
}

FbMainFrame::FbMainFrame()
{
	Create(NULL, wxID_ANY, GetTitle());
}

FbMainFrame::~FbMainFrame()
{
	wxSize size = GetSize();
	FbParams::Set(FB_FRAME_MAXIMIZE, IsMaximized());
	FbParams::Set(FB_FRAME_WIDTH, size.x);
	FbParams::Set(FB_FRAME_HEIGHT, size.y);
	SaveFrameList();
	m_FrameManager.UnInit();
}

void FbMainFrame::SaveFrameList()
{
	wxString frames;
	wxWindowID selected = 0;
	wxWindowID last_id = 0;
	size_t index = GetNotebook()->GetSelection();
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		wxWindowID id = GetNotebook()->GetPage(i)->GetId();
		if (ID_FRAME_AUTH <= id && id < ID_FRAME_FIND) {
			if (!frames.IsEmpty()) frames << wxT(',');
			frames << (id - ID_FRAME_AUTH);
			if (i == 0 || i == index) selected = id;
			last_id = id;
		}
	}
	if (selected && selected != last_id) frames << wxT(',') << (selected - ID_FRAME_AUTH);
	FbParams::Set(FB_FRAME_LIST, frames);
}

void FbMainFrame::RestoreFrameList()
{
	wxString frames = FbParams::GetStr(FB_FRAME_LIST);
	if (frames.IsEmpty()) frames = wxT('0');
	wxStringTokenizer tkz(frames, wxT(','), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		long id = 0;
		wxString text = tkz.GetNextToken();
		if (text.ToLong(&id) && 0 <= id && id < ID_FRAME_FIND - ID_FRAME_AUTH)
			FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_FRAME_AUTH + id).Post(this);
	}
}

bool FbMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
	wxSize size;
	bool maximized = FbParams::GetInt(FB_FRAME_MAXIMIZE);
	if (maximized) {
		size = wxSize( FbParams::DefaultInt(FB_FRAME_WIDTH), FbParams::DefaultInt(FB_FRAME_HEIGHT) );
	} else {
		size = wxSize( FbParams::GetInt(FB_FRAME_WIDTH), FbParams::GetInt(FB_FRAME_HEIGHT) );
	}

	bool res = wxAuiMDIParentFrame::Create(parent, id, title, wxDefaultPosition, size, wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU);
	if(res)	{
		if (maximized) Maximize();
		CreateControls();
		SetAccelerators();
		LoadIcon();
		if (maximized) {
			size.x = GetBestSize().x;
			SetSize(size);
		}
	}
	return res;
}


void FbMainFrame::SetAccelerators()
{
	wxAcceleratorEntry entries[] = {
		wxAcceleratorEntry(wxACCEL_CTRL, (int) WXK_F4, wxID_CLOSE),
		wxAcceleratorEntry(wxACCEL_CTRL, (int) WXK_INSERT, wxID_COPY),
	};
	wxAcceleratorTable accel(sizeof(entries) / sizeof(wxAcceleratorEntry), entries);
	SetAcceleratorTable(accel);
}

void FbMainFrame::LoadIcon()
{
	#ifdef __WIN32__
	wxIcon icon(wxT("aaaa"));
	SetIcon(icon);
	#else
	FbLogoBitmap bitmap;
	wxIcon icon;
	icon.CopyFromBitmap(bitmap);
	SetIcon(icon);
	#endif
}

void FbMainFrame::CreateControls()
{
	const int widths[] = {-90, -50, -50, -10};
	m_ProgressBar.Create(this, ID_PROGRESSBAR);
	m_ProgressBar.SetFieldsCount(4);
	m_ProgressBar.SetStatusWidths(4, widths);
	SetStatusBar(&m_ProgressBar);

	m_LOGTextCtrl.Create(this, ID_LOG_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER|wxTE_DONTWRAP);

//	long flags = wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE | wxNO_BORDER;
//	GetNotebook()->SetWindowStyleFlag(flags);
//	GetNotebook()->SetSelection(0);

	m_FrameManager.SetManagedWindow(this);
	SetTabArt(FbParams::GetInt(FB_NOTEBOOK_ART) + ID_ART_DEFAULT);

	SetToolBar(CreateToolBar());

	m_FrameManager.AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
	m_FrameManager.AddPane(&m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Log")).Caption(_("Info messages")).Show(false));
	m_FrameManager.Update();

	m_FindAuthor->SetFocus();

	Layout();
	Centre();

	RestoreFrameList();
}

void FbMainFrame::OnTabArt(wxCommandEvent & event)
{
	int id = event.GetId();
	SetTabArt(event.GetId());
	FbParams::Set(FB_NOTEBOOK_ART, id - ID_ART_DEFAULT);
}

void FbMainFrame::OnTabArtUpdate(wxUpdateUIEvent& event)
{
	int id = FbParams::GetInt(FB_NOTEBOOK_ART) + ID_ART_DEFAULT;
	if ( event.GetId() == id ) event.Check(true);
}

void FbMainFrame::SetTabArt(int id)
{
	wxAuiTabArt * art;
	switch (id) {
		case ID_ART_STANDART: art = new wxAuiDefaultTabArt; break;
		case ID_ART_COMPACT:  art = new FbCompactTabArt; break;
		case ID_ART_BUTTONS:  art = new FbToolbarTabArt; break;
		case ID_ART_TOOLBAR:  art = new FbToolbarTabArt(true); break;
		case ID_ART_VSTUDIO:  art = new FbVstudioTabArt; break;
		case ID_ART_MOZILLA:  art = new FbMozillaTabArt; break;
		default: art = new FbDefaultTabArt;
	}
	GetNotebook()->SetTabCtrlHeight(0);
	GetNotebook()->SetArtProvider(art);
	GetNotebook()->SetTabCtrlHeight(-1);
}

void FbMainFrame::OnSetup(wxCommandEvent & event)
{
	FbParamsDlg::Execute(this);
}

void FbMainFrame::OnMenuConfig(wxCommandEvent& event)
{
	FbConfigDlg::Execute(this);
	SetTitle(GetTitle());
}

void FbMainFrame::OnOpenWeb(wxCommandEvent & event)
{
	wxLaunchDefaultBrowser(MyRuLib::HomePage());
}

void FbMainFrame::OnAbout(wxCommandEvent & event)
{
	FbAboutDlg about(this);
	about.ShowModal();
}

wxToolBar * FbMainFrame::CreateToolBar()
{
	m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT);
	wxFont font = FbParams::GetFont(FB_FONT_TOOL);

	m_toolbar->AddTool(wxID_NEW, _("Import file"), wxArtProvider::GetBitmap(wxART_NEW), _("Import files to the library"));
	m_toolbar->AddTool(wxID_OPEN, _("Import folder"), wxArtProvider::GetBitmap(wxART_FILE_OPEN), _("Import folder to the library"));
	m_toolbar->AddSeparator();

	wxStaticText * text1 = new wxStaticText( m_toolbar, wxID_ANY, _(" Author: "), wxDefaultPosition, wxDefaultSize, 0 );
	text1->Wrap( -1 );
	text1->SetFont(font);
	m_toolbar->AddControl( text1 );

	m_FindAuthor = new wxTextCtrl(m_toolbar, ID_AUTHOR_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindAuthor->SetFont(font);
	m_toolbar->AddControl( m_FindAuthor );
	m_toolbar->AddTool(ID_AUTHOR_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND), _("Find author"));
	m_toolbar->AddSeparator();

	wxStaticText * text2 = new wxStaticText( m_toolbar, wxID_ANY, _(" Book: "), wxDefaultPosition, wxDefaultSize, 0 );
	text2->Wrap( -1 );
	text2->SetFont(font);
	m_toolbar->AddControl( text2 );

	m_FindTitle = new wxTextCtrl(m_toolbar, ID_TITLE_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindTitle->SetFont(font);
	m_toolbar->AddControl( m_FindTitle );
	m_toolbar->AddTool(ID_TITLE_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND), _("Find book by title"));
	m_toolbar->AddSeparator();

	m_toolbar->AddTool(ID_MODE_TREE, _("Hierarchy"), wxArtProvider::GetBitmap(wxART_LIST_VIEW), _("Hierarchy of authors and series"));
	m_toolbar->AddTool(ID_MODE_LIST, _("List"), wxArtProvider::GetBitmap(wxART_REPORT_VIEW), _("Simple list"));
	m_toolbar->AddSeparator();
	m_toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Export to external device"));
	m_toolbar->Realize();

	m_toolbar->SetFont(font);

	return m_toolbar;
}

void FbMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void FbMainFrame::OnNewZip( wxCommandEvent& event )
{
	wxString mask;
	mask << _("Digital books and archives") << (wxString)wxT(" (*.fb2; *.zip)|*.zip;*.Zip;*.ZIP;*.fb2;*.Fb2;*.FB2");
	mask << wxT('|') << _("Digital books in FB2") << (wxString)wxT(" (*.fb2)|*.fb2;*.Fb2;*.FB2");
	mask << wxT('|') << _("ZIP archives") << (wxString)wxT(" (*.zip)|*.zip;*.Zip;*.ZIP");
	mask << wxT('|') << _("All files") << (wxString)wxT(" (*.*)|*.*");

	wxFileDialog dlg (
		this,
		_("Select archive to add to the library"),
		wxEmptyString,
		wxEmptyString,
		mask,
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths(paths);
		(new FbZipImportThread(this, paths))->Execute();
	}
}

void FbMainFrame::OnFolder( wxCommandEvent& event ) {

	wxDirDialog dlg (
		this,
		_("Select folder to recursive import"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
		wxString path = dlg.GetPath();
		(new FbDirImportThread(this, path))->Execute();
	}
}

void FbMainFrame::OnProgressUpdate(wxUpdateUIEvent& event)
{
	m_ProgressBar.SetProgress(event.GetInt());
	m_ProgressBar.SetStatusText(event.GetText(), 0);
	m_ProgressBar.SetStatusText(event.GetString(), 2);
}

void FbMainFrame::OnError(wxCommandEvent& event)
{
	ShowLog(true);
	m_LOGTextCtrl.AppendText(event.GetString() + wxT("\n"));
}

wxAuiPaneInfo * FbMainFrame::FindLog()
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i) {
		wxAuiPaneInfo & info = all_panes.Item(i);
		if (info.name == wxT("Log")) return &info;
	}
	return NULL;
}

void FbMainFrame::ShowLog(bool forced)
{
	wxAuiPaneInfo * info = FindLog();
	if (info) {
		bool show = forced || !info->IsShown();
		if (show != info->IsShown() && FbParams::GetInt(FB_CLEAR_LOG)) m_LOGTextCtrl.Clear();
		info->Show(show);
		m_FrameManager.Update();
	}
}

void FbMainFrame::OnHideLog(wxCommandEvent& event)
{
	ShowLog();
}

void FbMainFrame::OnFindTitle(wxCommandEvent & event)
{
	FindTitle(m_FindTitle->GetValue(), m_FindAuthor->GetValue());
}

void FbMainFrame::OnFindTitleEnter(wxCommandEvent& event)
{
	FindTitle(event.GetString(), m_FindAuthor->GetValue());
}

void FbMainFrame::FindTitle(const wxString &title, const wxString &author)
{
	if (!title.IsEmpty()) {
		FbMasterFindInfo info(title, author);
		wxString text = _("Search"); text << COLON << title;
		OpenInfo(info, text);
	}
}

void FbMainFrame::OnFindAuthor(wxCommandEvent& event)
{
	FindAuthor(m_FindAuthor->GetValue());
}

void FbMainFrame::OnFindAuthorEnter(wxCommandEvent& event)
{
	FindAuthor(event.GetString());
}

void FbMainFrame::FindAuthor(const wxString &text)
{
	FbFrameAuth * authors = wxDynamicCast(FindFrameById(ID_FRAME_AUTH, true), FbFrameAuth);
	if (!authors) {
		authors = new FbFrameAuth(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		authors->Update();
	}
	authors->FindAuthor(text);
	authors->ActivateAuthors();
}

void FbMainFrame::OnMenuAuthor(wxCommandEvent& event)
{
	FindAuthor(wxEmptyString);
}

void FbMainFrame::OnMenuTitle(wxCommandEvent& event)
{
	wxString text = wxGetTextFromUser(_("Input search string:"), _("Find title"));
	if (text.IsEmpty()) return;
	FindTitle(text, wxEmptyString);
}

void FbMainFrame::OnMenuFrame(wxCommandEvent & event)
{
	wxWindow * frame = FindFrameById(event.GetId(), true);
	if ( frame ) return;

	switch ( event.GetId() ) {
		case ID_FRAME_AUTH: {
			FbFrameAuth * authors = new FbFrameAuth(this);
			authors->ActivateAuthors();
			authors->Update();
			frame = authors;
		} break;
		case ID_FRAME_GENR: {
			frame = new FbFrameGenr(this);
		} break;
		case ID_FRAME_FLDR: {
			frame = new FbFrameFldr(this);
		} break;
		case ID_FRAME_DOWN: {
			frame = new FbFrameDown(this);
		} break;
		case ID_FRAME_SEQN: {
			frame = new FbFrameSeqn(this);
		} break;
		case ID_FRAME_DATE: {
			frame = new FbFrameDate(this);
		} break;
	}
	if (frame) frame->Update();
}

wxWindow * FbMainFrame::FindFrameById(const int id, bool bActivate)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		if (GetNotebook()->GetPage(i)->GetId() == id) {
			wxWindow * result = GetNotebook()->GetPage(i);
			if (bActivate) GetNotebook()->SetSelection(i);
			return result;
		}
	}
	return NULL;
}

void FbMainFrame::OnMenuNothing(wxCommandEvent& event)
{
	wxMessageBox(_("This function is not available yet in this version."));
}

void FbMainFrame::OnDatabaseInfo(wxCommandEvent & event)
{
	FbFrameInfo::Execute(this);
}

void FbMainFrame::OnVacuum(wxCommandEvent & event)
{
//	wxString msg = _("Optimize database?");
}

void FbMainFrame::OnUpdate(wxCommandEvent & event)
{
	FbUpdateThread * thread = new FbUpdateThread;
	thread->Execute();
}

void FbMainFrame::OnUpdateUpdate(wxUpdateUIEvent& event)
{
	int code = FbParams::GetInt(DB_DATAFILE_DATE);
	event.Enable(code && code < FbDateTime::Today().Code() + 20000000);
}

void FbMainFrame::OnUpdateFolder(FbFolderEvent & event)
{
	if (event.m_type == FT_DOWNLOAD) {
		if (FbParams::GetInt(FB_AUTO_DOWNLD)) wxGetApp().StartDownload();
		FbFrameDown * frame = wxDynamicCast(FindFrameById(ID_FRAME_DOWN, false), FbFrameDown);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);

		size_t count = GetNotebook()->GetPageCount();
		for (size_t i = 0; i < count; ++i) {
			FbFrameBase * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbFrameBase);
			if (frame) frame->RefreshBooks();
		}
	} else {
		FbFrameFldr * frame = wxDynamicCast(FindFrameById(ID_FRAME_FLDR, false), FbFrameFldr);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);
	}
}

void FbMainFrame::OnUpdateMaster(FbMasterEvent & event)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameBase * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbFrameBase);
		if (frame) frame->UpdateMaster(event);
	}
}

void FbMainFrame::OnOpenAuthor(FbOpenEvent & event)
{
	FbMasterAuthInfo info(event.m_author);
	wxString text = FbCollection::GetAuth(event.m_author, 0);
	OpenInfo(info, text);
}

void FbMainFrame::OnOpenSequence(FbOpenEvent & event)
{
	FbMasterSeqnInfo info(event.m_author);
	wxString text = FbCollection::GetSeqn(event.m_author, 0);
	OpenInfo(info, text);
}

void FbMainFrame::OpenInfo(const FbMasterInfo & info, const wxString & text)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameFind * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbFrameFind);
		if (frame && info == frame->GetInfo()) {
			GetNotebook()->SetSelection(i);
			return;
		}
	}
	new FbFrameFind(this, info, text);
}

void FbMainFrame::OnInfoCommand(wxCommandEvent & event)
{
	FbFrameInfo * frame = wxDynamicCast(FindFrameById(ID_FRAME_INFO, true), FbFrameInfo);
	if (!frame) {
		frame = new FbFrameInfo(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
	frame->Load(event.GetString());
}

void FbMainFrame::OnProgress(FbProgressEvent & event)
{
	m_ProgressBar.SetProgress(event.m_pos);
	m_ProgressBar.SetStatusText(event.m_str, 0);
	m_ProgressBar.SetStatusText(event.m_text, 2);
}

void FbMainFrame::OnUpdateBook(wxCommandEvent & event)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameBase * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbFrameBase);
		if (frame) frame->UpdateInfo(event.GetInt());
	}
}

void FbMainFrame::OnDatabaseOpen(wxCommandEvent & event)
{
	wxString filename = FbDataOpenDlg::Execute(this);
	if (!filename.IsEmpty()) OpenDatabase(filename);
}

void FbMainFrame::OnUpdateFonts(wxCommandEvent & event)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbAuiMDIChildFrame * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbAuiMDIChildFrame);
		if (frame) frame->UpdateFonts();
	}
}

void FbMainFrame::OnMenuRecent(wxCommandEvent & event)
{
	int param = event.GetId() - wxID_FILE + FB_RECENT_0;
	wxString filename = FbParams::GetStr(param);
	if (filename.IsEmpty()) return;

	if (wxFileName::FileExists(filename)) {
		OpenDatabase(filename);
	} else {
		wxLogWarning(_("File not found: ") + filename);
		FbParams::Set(param, wxEmptyString);
	}
}

void FbMainFrame::OnRecentUpdate(wxUpdateUIEvent& event)
{
	wxMenuBar * menubar = GetMenuBar();
	if (!menubar) return;

	wxMenuItem * menuitem = menubar->FindItem(wxID_FILE);
	if (!menuitem) return;

	wxMenu * submenu = menuitem->GetSubMenu();
	if (!submenu) return;

	wxMenuItemList items = submenu->GetMenuItems();
	for (size_t i = 0; i < items.GetCount(); i++) {
		submenu->Delete(items[i]);
	}

	for (size_t i = 1; i<=5; i++) {
		wxString filename = FbParams::GetStr(i + FB_RECENT_0);
		if (filename.IsEmpty()) continue;
		wxString fileinfo = FbParams::GetStr(i + FB_TITLE_0);
		submenu->Append(wxID_FILE + i, filename, fileinfo);
	}
}

void FbMainFrame::OpenDatabase(const wxString &filename)
{
	if (wxGetApp().OpenDatabase(filename)) {
		SetTitle(GetTitle());
		SaveFrameList();
		while (GetNotebook()->GetPageCount()) delete GetNotebook()->GetPage(0);
		RestoreFrameList();
	}
}

void FbMainFrame::SetStatus(const wxString &text)
{
	m_ProgressBar.SetStatusText(text, 2);
}

void FbMainFrame::OnFullScreen(wxCommandEvent& event)
{
	bool show = !IsFullScreen();
	long style = wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;
	ShowFullScreen(show, style);
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbAuiMDIChildFrame * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbAuiMDIChildFrame);
		if (frame) frame->ShowFullScreen(show);
	}
}

void FbMainFrame::OnFullScreenUpdate(wxUpdateUIEvent& event)
{
	event.Check(IsFullScreen());
}

void FbMainFrame::OnHideLogUpdate(wxUpdateUIEvent& event)
{
	wxAuiPaneInfo * info = FindLog();
	event.Check( info && info->IsShown() );
}

void FbMainFrame::OnWindowClose(wxCommandEvent & event)
{
	if (GetActiveChild()) GetActiveChild()->Close();
}

void FbMainFrame::OnWindowCloseAll(wxCommandEvent & event)
{
	while (GetActiveChild()) GetActiveChild()->Close();
}

void FbMainFrame::OnWindowNext(wxCommandEvent & event)
{
	ActivateNext();
}

void FbMainFrame::OnWindowPrev(wxCommandEvent & event)
{
	ActivatePrevious();
}

void FbMainFrame::Localize(int language)
{
    wxToolBar * toolbar = m_toolbar;
	SetToolBar(CreateToolBar());
    wxDELETE(toolbar);

	size_t count = GetNotebook()->GetPageCount();
	size_t index = GetNotebook()->GetSelection();
	for (size_t i = 0; i < count; ++i) {
		FbAuiMDIChildFrame * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbAuiMDIChildFrame);
		if (frame) frame->Localize(i == index);
	}
}

void FbMainFrame::OnDatabaseGenres(wxCommandEvent & event)
{
	(new FbGenreThread)->Execute();
}
