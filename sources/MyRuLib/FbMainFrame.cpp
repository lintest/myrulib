#include "FbMainFrame.h"
#include <wx/splitter.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include <wx/stdpaths.h>
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "dialogs/FbParamsDlg.h"
#include "dialogs/FbDataOpenDlg.h"
#include "dialogs/FbDirImportDlg.h"
#include "dialogs/FbConfigDlg.h"
#include "dialogs/FbAboutDlg.h"
#include "dialogs/FbProgressDlg.h"
#include "dialogs/FbReaderDlg.h"
#include "FbImportThread.h"
#include "frames/FbCoolReader.h"
#include "frames/FbFrameAuth.h"
#include "frames/FbFrameClss.h"
#include "frames/FbFrameFind.h"
#include "frames/FbFrameFldr.h"
#include "frames/FbFrameGenr.h"
#include "frames/FbFrameInfo.h"
#include "frames/FbFrameDate.h"
#include "frames/FbFrameDown.h"
#include "frames/FbFrameSeqn.h"
#include "frames/FbFrameHtml.h"
#include "FbDownloader.h"
#include "FbLogoBitmap.h"
#include "FbMainMenu.h"
#include "FbMasterInfo.h"
#include "FbGenreThread.h"
#include "FbUpdateThread.h"
#include "FbMasterTypes.h"
#include "controls/FbLogModel.h"
#include "controls/FbSearchCtrl.h"
#include "controls/ProgressBar.h"
#include "FbDateTime.h"
#include "FbLocale.h"
#include "FbString.h"

IMPLEMENT_CLASS(FbMainFrame, wxFrame)

BEGIN_EVENT_TABLE(FbMainFrame, wxFrame)

	EVT_MENU(wxID_ANY, FbMainFrame::OnClassMenu)

	EVT_MENU(wxID_NEW, FbMainFrame::OnNewZip)
	EVT_MENU(wxID_OPEN, FbMainFrame::OnFolder)
	EVT_MENU(wxID_EXIT, FbMainFrame::OnExit)
	EVT_MENU(wxID_PREFERENCES, FbMainFrame::OnSetup)
	EVT_MENU(ID_READER_OPTIONS, FbMainFrame::OnReader)
	EVT_MENU(wxID_ABOUT, FbMainFrame::OnAbout)
	EVT_MENU_RANGE(wxID_FILE1, wxID_FILE5, FbMainFrame::OnMenuRecent)

	EVT_UPDATE_UI(wxID_NEW, FbMainFrame::OnReadOnlyUpdate)
	EVT_UPDATE_UI(wxID_OPEN, FbMainFrame::OnReadOnlyUpdate)
	EVT_UPDATE_UI(ID_MENU_VACUUM, FbMainFrame::OnReadOnlyUpdate)
	EVT_UPDATE_UI(ID_MENU_UPDATE, FbMainFrame::OnReadOnlyUpdate)

	EVT_SEARCHCTRL_SEARCH_BTN(ID_AUTHOR_TXT, FbMainFrame::OnFindAuthor)
	EVT_SEARCHCTRL_SEARCH_BTN(ID_TITLE_TXT, FbMainFrame::OnFindTitle)

	EVT_UPDATE_UI(wxID_FILE, FbMainFrame::OnRecentUpdate)

	EVT_MENU(wxID_SAVE, FbMainFrame::OnSubmenu)
	EVT_MENU(wxID_FIND, FbMainFrame::OnSubmenu)

	EVT_MENU(wxID_CUT, FbMainFrame::OnSubCtrl)
	EVT_MENU(wxID_COPY, FbMainFrame::OnSubCtrl)
	EVT_MENU(wxID_PASTE, FbMainFrame::OnSubCtrl)
	EVT_MENU(ID_UNSELECTALL, FbMainFrame::OnSubCtrl)
	EVT_MENU(wxID_SELECTALL, FbMainFrame::OnSubCtrl)
	EVT_MENU(wxID_DELETE, FbMainFrame::OnDelete)

	EVT_UPDATE_UI(wxID_CUT, FbMainFrame::OnEnableUI)
	EVT_UPDATE_UI(wxID_COPY, FbMainFrame::OnEnableUI)
	EVT_UPDATE_UI(wxID_SELECTALL, FbMainFrame::OnEnableUI)
	EVT_UPDATE_UI(ID_UNSELECTALL, FbMainFrame::OnEnableUI)
	EVT_UPDATE_UI(ID_MASTER_APPEND, FbMainFrame::OnDisableUI)
	EVT_UPDATE_UI(ID_MASTER_MODIFY, FbMainFrame::OnDisableUI)
	EVT_UPDATE_UI(ID_MASTER_REPLACE, FbMainFrame::OnDisableUI)
	EVT_UPDATE_UI(ID_MASTER_PAGE, FbMainFrame::OnDisableUI)

	EVT_MENU(ID_MENU_SEARCH, FbMainFrame::OnMenuTitle)
	EVT_MENU_RANGE(ID_FRAME_AUTH, ID_FRAME_DATE, FbMainFrame::OnMenuFrame)
	EVT_COMMAND_RANGE(ID_FRAME_AUTH, ID_FRAME_DATE, fbEVT_INIT_FRAMES, FbMainFrame::OnInitFrame)
	EVT_MENU(ID_FRAME_ARCH, FbMainFrame::OnMenuNothing)

	EVT_MENU(ID_MENU_DB_INFO, FbMainFrame::OnDatabaseInfo)
	EVT_MENU(ID_MENU_DB_OPEN, FbMainFrame::OnDatabaseOpen)
	EVT_MENU(ID_MENU_GENRES, FbMainFrame::OnDatabaseGenres)
	EVT_MENU(ID_MENU_VACUUM, FbMainFrame::OnVacuum)
	EVT_MENU(ID_MENU_UPDATE, FbMainFrame::OnUpdate)
	EVT_MENU(ID_MENU_CONFIG, FbMainFrame::OnMenuConfig)
	EVT_MENU(ID_OPEN_WEB, FbMainFrame::OnOpenWeb)

	EVT_MENU(ID_AUTHOR_BTN, FbMainFrame::OnFindAuthor)
	EVT_MENU(ID_TITLE_BTN, FbMainFrame::OnFindTitle)
	EVT_TEXT_ENTER(ID_AUTHOR_TXT, FbMainFrame::OnFindAuthor)
	EVT_TEXT_ENTER(ID_TITLE_TXT, FbMainFrame::OnFindTitle)

	EVT_MENU(ID_PROGRESS_FINISH, FbMainFrame::OnImportFinish)
	EVT_UPDATE_UI(ID_PROGRESS_UPDATE, FbMainFrame::OnProgressUpdate)

	EVT_MENU_RANGE(ID_SUBMENU_FIRST, ID_SUBMENU_LAST, FbMainFrame::OnSubmenu)
	EVT_MENU(wxID_VIEW_SORTNAME, FbMainFrame::OnSubmenu)
	EVT_MENU(wxID_VIEW_SORTDATE, FbMainFrame::OnSubmenu)
	EVT_MENU(wxID_VIEW_SORTSIZE, FbMainFrame::OnSubmenu)
	EVT_MENU(wxID_VIEW_SORTTYPE, FbMainFrame::OnSubmenu)

	EVT_UPDATE_UI(ID_MODE_LIST, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_MODE_TREE, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_SPLIT_NOTHING, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_USE, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_FILTER_DEL, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_DIRECTION, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTNAME, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTDATE, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTSIZE, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(wxID_VIEW_SORTTYPE, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_AUTHOR, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_RATING, FbMainFrame::OnSubmenuUpdateUI)
	EVT_UPDATE_UI(ID_ORDER_LANG, FbMainFrame::OnSubmenuUpdateUI)

	EVT_MENU(ID_TEXTLOG_SHOW, FbMainFrame::OnHideLog)
	EVT_MENU(ID_TEXTLOG_HIDE, FbMainFrame::OnHideLog)
	EVT_MENU(ID_UPDATE_FONTS, FbMainFrame::OnUpdateFonts)
	EVT_MENU(ID_FULLSCREEN, FbMainFrame::OnFullScreen)
	EVT_MENU(ID_STATUS_SHOW, FbMainFrame::OnStatusBar)
	EVT_MENU_RANGE(ID_ART_DEFAULT, ID_ART_MOZILLA, FbMainFrame::OnTabArt)

	EVT_UPDATE_UI(ID_MENU_UPDATE, FbMainFrame::OnUpdateUpdate)
	EVT_UPDATE_UI(ID_FULLSCREEN,  FbMainFrame::OnFullScreenUpdate)
	EVT_UPDATE_UI(ID_TEXTLOG_SHOW, FbMainFrame::OnHideLogUpdate)
	EVT_UPDATE_UI(ID_STATUS_SHOW, FbMainFrame::OnStatusBarUpdate)
	EVT_UPDATE_UI_RANGE(ID_ART_DEFAULT, ID_ART_MOZILLA, FbMainFrame::OnTabArtUpdate)

	EVT_MENU(wxID_CLOSE, FbMainFrame::OnWindowClose)
	EVT_MENU(wxID_CLOSE_ALL, FbMainFrame::OnWindowCloseAll)
	EVT_MENU(ID_WINDOW_NEXT, FbMainFrame::OnWindowNext)
	EVT_MENU(ID_WINDOW_PREV, FbMainFrame::OnWindowPrev)

	EVT_FB_OPEN(ID_BOOK_AUTH, FbMainFrame::OnOpenAuth)
	EVT_FB_OPEN(ID_BOOK_SEQN, FbMainFrame::OnOpenSeqn)
	EVT_FB_FOLDER(ID_UPDATE_FOLDER, FbMainFrame::OnUpdateFolder)
	EVT_FB_MASTER(ID_UPDATE_MASTER, FbMainFrame::OnUpdateMaster)
	EVT_FB_PROGRESS(ID_PROGRESS_UPDATE, FbMainFrame::OnProgress)
	EVT_COMMAND(ID_DATABASE_INFO, fbEVT_BOOK_ACTION, FbMainFrame::OnInfoCommand)
	EVT_COMMAND(ID_UPDATE_BOOK, fbEVT_BOOK_ACTION, FbMainFrame::OnUpdateBook)
	EVT_COMMAND(ID_FOUND_NOTHING, fbEVT_BOOK_ACTION, FbMainFrame::OnFoundNothing)

	EVT_NAVIGATION_KEY(FbMainFrame::OnNavigationKey)

	EVT_AUI_PANE_CLOSE(FbMainFrame::OnPaneClose)
	EVT_AUINOTEBOOK_ALLOW_DND(ID_AUI_NOTEBOOK, FbMainFrame::OnAllowNotebookDnD)
	EVT_AUINOTEBOOK_PAGE_CHANGED(ID_AUI_NOTEBOOK, FbMainFrame::OnNotebookChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSED(ID_AUI_NOTEBOOK, FbMainFrame::OnNotebookClosed)
	EVT_IDLE(FbMainFrame::OnIdle)
END_EVENT_TABLE()

wxString FbMainFrame::GetTitle() const
{
	return FbParams(DB_LIBRARY_TITLE).Str() + wxT(" - ") + MyRuLib::ProgramName();
}

FbMainFrame::FbMainFrame()
	: m_ProgressBar(NULL)
	, m_MenuBook(NULL)
	, m_MenuTree(NULL)
	, m_MenuRead(NULL)
{
	Create(NULL, wxID_ANY, GetTitle());
}

FbMainFrame::~FbMainFrame()
{
	wxSize size = GetSize();
	FbParams(FB_FRAME_MAXIMIZE) = IsMaximized();
	FbParams(FB_FRAME_WIDTH) = size.x;
	FbParams(FB_FRAME_HEIGHT) = size.y;
	SaveFrameList();
	m_FrameManager.UnInit();
	wxDELETE(m_MenuBook);
}

void FbMainFrame::OnClassMenu(wxCommandEvent& event)
{
	int code;
	FbMenu::Type type;
	if (FbMenuItem::Get(event.GetId(), type, code) && type == FbMenu::CLSS) {
		OpenClss(code);
		return;
	}
	event.Skip();
}

void FbMainFrame::OnSubmenu(wxCommandEvent& event)
{
	wxWindow * child = GetActiveChild();
	if (FbFrameBase * frame = wxDynamicCast(child, FbFrameBase)) {
		frame->DoEvent(event);
	} else if (FbFrameHtml * frame = wxDynamicCast(child, FbFrameHtml)) {
		frame->DoEvent(event);
	} else if (FbFrameInfo * frame = wxDynamicCast(child, FbFrameInfo)) {
		frame->DoEvent(event);
#ifdef FB_INCLUDE_READER
	} else if (FbCoolReader * frame = wxDynamicCast(child, FbCoolReader)) {
		frame->DoEvent(event);
#endif // FB_INCLUDE_READER
	}
}

void FbMainFrame::OnSubmenuUpdateUI(wxUpdateUIEvent & event)
{
	wxWindow * child = GetActiveChild();
	if (FbFrameBase * frame = wxDynamicCast(child, FbFrameBase)) {
		frame->DoEvent(event);
	} else if (FbFrameHtml * frame = wxDynamicCast(child, FbFrameHtml)) {
		frame->DoEvent(event);
	} else if (FbFrameInfo * frame = wxDynamicCast(child, FbFrameInfo)) {
		frame->DoEvent(event);
#ifdef FB_INCLUDE_READER
	} else if (FbCoolReader * frame = wxDynamicCast(child, FbCoolReader)) {
		frame->DoEvent(event);
#endif // FB_INCLUDE_READER
	}
}

void FbMainFrame::OnSubCtrl(wxCommandEvent& event)
{
	if (wxWindow * focused = FindFocus()) {
		if (focused == this) return;
		wxPostEvent(focused, event);
	}
}

void FbMainFrame::OnDelete(wxCommandEvent& event)
{
	wxWindow * focused = FindFocus(); if (!focused) return;
	if (FbTreeViewCtrl * tree = wxDynamicCast(focused->GetParent(), FbTreeViewCtrl)) { tree->DoEvent(event); return; }
}

void FbMainFrame::SaveFrameList()
{
	wxString frames;
	wxWindowID selected = 0;
	wxWindowID last_id = 0;
	size_t index = m_FrameNotebook.GetSelection();
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		wxWindowID id = m_FrameNotebook.GetPage(i)->GetId();
		if (ID_FRAME_AUTH <= id && id < ID_FRAME_FIND) {
			if (!frames.IsEmpty()) frames << wxT(',');
			frames << (id - ID_FRAME_AUTH);
			if (i == 0 || i == index) selected = id;
			last_id = id;
		}
	}
	if (selected && selected != last_id) frames << wxT(',') << (selected - ID_FRAME_AUTH);
	FbParams(FB_FRAME_LIST) = frames;
}

void FbMainFrame::RestoreFrameList()
{
	bool exists = false;
	wxString frames = FbParams(FB_FRAME_LIST);
	if (frames.IsEmpty()) frames = wxT('0');
	wxString active = frames.AfterLast(wxT(','));
	wxStringTokenizer tkz(frames, wxT(','), wxTOKEN_STRTOK);
	while (tkz.HasMoreTokens()) {
		long id = 0;
		wxString text = tkz.GetNextToken();
		if (exists && text == active) break;
		if (text.ToLong(&id) && 0 <= id && id < ID_FRAME_FIND - ID_FRAME_AUTH)
			FbCommandEvent(fbEVT_INIT_FRAMES, ID_FRAME_AUTH + id, text == active).Post(this);
		if (!exists) exists = (text == active);
	}
}

bool FbMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
#ifdef __WXMAC__
    wxSize size = wxSize(800,600);
    bool maximized = false;
#else
    wxSize size = wxDefaultSize;
    bool maximized = FbParams(FB_FRAME_MAXIMIZE);
	if (maximized) {
		size = wxSize( FbParamItem::DefaultInt(FB_FRAME_WIDTH), FbParamItem::DefaultInt(FB_FRAME_HEIGHT) );
	} else {
		size = wxSize( FbParams(FB_FRAME_WIDTH), FbParams(FB_FRAME_HEIGHT) );
	}
#endif // __WXMAC__

	bool res = wxFrame::Create(parent, id, title, wxDefaultPosition, size, wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU);
	if(res)	{
		wxMenuBar * menubar = new FbMenuBar;
		m_MenuTree = menubar->GetMenu(fbBOOK_MENU_POSITION);
		SetMenuBar(menubar);
		SetMinSize(wxSize(400,300));
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
		wxAcceleratorEntry(wxACCEL_CTRL , (int) wxT('Q')   , wxID_EXIT      ),
		wxAcceleratorEntry(wxACCEL_CTRL , (int) wxT('F')   , wxID_FIND      ),
		wxAcceleratorEntry(wxACCEL_CTRL , (int) WXK_F4     , wxID_CLOSE     ),
		wxAcceleratorEntry(wxACCEL_SHIFT, (int) WXK_DELETE , wxID_CUT       ),
		wxAcceleratorEntry(wxACCEL_CTRL , (int) WXK_INSERT , wxID_COPY      ),
		wxAcceleratorEntry(wxACCEL_SHIFT, (int) WXK_INSERT , wxID_PASTE     ),
		wxAcceleratorEntry(wxACCEL_CTRL , (int) WXK_TAB    , ID_WINDOW_NEXT ),
		wxAcceleratorEntry(wxACCEL_CTRL | wxACCEL_SHIFT, (int) WXK_TAB, ID_WINDOW_PREV),
	};
	wxAcceleratorTable accel(sizeof(entries) / sizeof(wxAcceleratorEntry), entries);
	SetAcceleratorTable(accel);
}

void FbMainFrame::LoadIcon()
{
	#ifdef __WXMSW__
	wxIcon icon(wxT("aaaa"));
	SetIcon(icon);
	#else  // __WXMSW__
	FbLogoBitmap bitmap;
	wxIcon icon;
	icon.CopyFromBitmap(bitmap);
	SetIcon(icon);
	#endif // __WXMSW__
}

void FbMainFrame::CreateStatusBar(bool show)
{
	const int widths[] = {-90, -50, -50, -10};
	m_ProgressBar = new ProgressBar(this, ID_PROGRESSBAR);
	m_ProgressBar->SetFieldsCount(4);
	m_ProgressBar->SetStatusWidths(4, widths);
	m_ProgressBar->Show(show);
	SetStatusBar(m_ProgressBar);
}

void FbMainFrame::CreateControls()
{
	SetToolBar(CreateToolBar());
	if (FbParams(FB_STATUS_SHOW)) {
		CreateStatusBar(true);
	}

	m_LogCtrl = new FbLogViewCtrl;
	m_LogCtrl->Create(this, ID_TEXTLOG_CTRL, wxDefaultPosition, wxSize(-1, 100), fbTR_MULTIPLE | fbTR_NO_HEADER);
	m_LogCtrl->AssignModel(new FbLogModel);

	m_FrameManager.SetManagedWindow(this);
	m_FrameNotebook.Create( this, ID_AUI_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxTAB_TRAVERSAL | wxNO_BORDER );

	SetTabArt(FbParams(FB_NOTEBOOK_ART) + ID_ART_DEFAULT);

	m_FrameManager.AddPane(&m_FrameNotebook, wxAuiPaneInfo().Name(wxT("Main")).CenterPane().PaneBorder(false));
	m_FrameManager.AddPane(m_LogCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Log")).Caption(_("Info messages")).Show(false));
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
	FbParams(FB_NOTEBOOK_ART) = id - ID_ART_DEFAULT;
}

void FbMainFrame::OnTabArtUpdate(wxUpdateUIEvent& event)
{
	int id = FbParams(FB_NOTEBOOK_ART) + ID_ART_DEFAULT;
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
	m_FrameNotebook.SetTabCtrlHeight(0);
	m_FrameNotebook.SetArtProvider(art);
	m_FrameNotebook.SetTabCtrlHeight(-1);
}

void FbMainFrame::OnSetup(wxCommandEvent & event)
{
	FbParamsDlg::Execute(this);
}

void FbMainFrame::OnReader(wxCommandEvent & event)
{
	#ifdef FB_INCLUDE_READER
	if (!FbReaderDlg::Execute(this)) return;
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbCoolReader * reader = wxDynamicCast(m_FrameNotebook.GetPage(i), FbCoolReader);
		if (reader) reader->Setup(true);
	}
	#endif // FB_INCLUDE_READER
}

void FbMainFrame::OnMenuConfig(wxCommandEvent& event)
{
	if (FbConfigDlg::Execute(this)) {
		SetTitle(GetTitle());
		UpdateMenuRefs();
	}
}

void FbMainFrame::UpdateMenuRefs()
{
	wxMenuBar * menubar = GetMenuBar();
	if (!menubar) return;

	wxMenu * menu = menubar->GetMenu(fbFIND_MENU_POSITION);
	if (!menu) return;

	wxMenuItem * item = menu->FindItem(ID_FRAME_CLSS);
	if (item) menu->Delete(item);

	wxMenuItem * submenu = FbMenuRefs::Create(menu);
	if (submenu) menu->Insert(1, submenu);
}

void FbMainFrame::OnOpenWeb(wxCommandEvent & event)
{
	wxLaunchDefaultBrowser(MyRuLib::HomePage());
}

void FbMainFrame::OnAbout(wxCommandEvent & event)
{
	FbAboutDlg(this).ShowModal();
}

#ifdef __WXMAC__

wxToolBar * FbMainFrame::CreateToolBar()
{
    wxToolBar * toolbar = wxFrame::CreateToolBar(wxTB_FLAT, wxID_ANY);
    toolbar->SetToolBitmapSize(wxSize(16, 16));
	wxFont font = FbParams(FB_FONT_TOOL);

	wxString path = wxStandardPaths::Get().GetResourcesDir() + wxT("/");
	toolbar->AddTool(wxID_NEW, _("Import file"), wxBitmap(path + wxT("document-new.png"), wxBITMAP_TYPE_PNG), _("Import files to the library"));
	toolbar->AddTool(wxID_OPEN, _("Import folder"), wxBitmap(path + wxT("document-open.png"), wxBITMAP_TYPE_PNG), _("Import folder to the library"));

	m_FindAuthor = new wxSearchCtrl(toolbar, ID_AUTHOR_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindAuthor->SetDescriptiveText(_("Author"));
	m_FindAuthor->ShowCancelButton(true);
	toolbar->AddControl( m_FindAuthor );

	m_FindTitle = new wxSearchCtrl(toolbar, ID_TITLE_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindTitle->SetDescriptiveText(_("Title"));
	m_FindTitle->ShowCancelButton(true);
	toolbar->AddControl( m_FindTitle );

	toolbar->AddTool(ID_MODE_TREE, _("Hierarchy"), wxBitmap(path + wxT("format-justify-right.png"), wxBITMAP_TYPE_PNG), _("Hierarchy of authors and series"));
	toolbar->AddTool(ID_MODE_LIST, _("List"), wxBitmap(path + wxT("format-justify-left.png"), wxBITMAP_TYPE_PNG), _("Simple list"));

	toolbar->AddTool(wxID_SAVE, _("Export"), wxBitmap(path + wxT("document-save.png"), wxBITMAP_TYPE_PNG), _("Export to external device"));

	toolbar->SetFont(font);
	toolbar->Realize();

	return toolbar;
}

#else

#ifdef __WXGTK__
	#define fbART_REPORT_VIEW wxT("gtk-justify-right")
	#define fbART_LIST_VIEW wxT("gtk-justify-left")
#else
	#define fbART_REPORT_VIEW wxART_REPORT_VIEW
	#define fbART_LIST_VIEW wxART_LIST_VIEW
#endif

wxToolBar * FbMainFrame::CreateToolBar()
{
	wxString textAuth = _(" Author: ");
	wxString textBook = _(" Book: ");

    wxToolBar * toolbar = wxFrame::CreateToolBar(wxTB_FLAT, wxID_ANY);
    toolbar->SetToolBitmapSize(wxSize(16, 16));
	wxFont font = FbParams(FB_FONT_TOOL);

	toolbar->AddTool(wxID_NEW, _("Import file"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR), _("Import files to the library"));
	toolbar->AddTool(wxID_OPEN, _("Import folder"), wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR), _("Import folder to the library"));

#ifdef FB_SEARCH_COMBO_CTRL
//	toolbar->AddSeparator();
#else
	textAuth.Prepend(wxT("  "));
	textBook.Prepend(wxT("  "));
#endif // __WXMSW__

	wxStaticText * text1 = new wxStaticText( toolbar, wxID_ANY, textAuth, wxDefaultPosition, wxDefaultSize, 0 );
	text1->Wrap( -1 );
	text1->SetFont(font);
	toolbar->AddControl( text1 );

	m_FindAuthor = new FbSearchCtrl(toolbar, ID_AUTHOR_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindAuthor->SetFont(font);
	toolbar->AddControl( m_FindAuthor );

#ifdef FB_SEARCH_COMBO_CTRL
	toolbar->AddTool(ID_AUTHOR_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR), _("Find author"));
	toolbar->AddSeparator();
#endif // __WXMSW__

	wxStaticText * text2 = new wxStaticText(toolbar, wxID_ANY, textBook, wxDefaultPosition, wxDefaultSize, 0 );
	text2->Wrap( -1 );
	text2->SetFont(font);
	toolbar->AddControl( text2 );

	m_FindTitle = new FbSearchCtrl(toolbar, ID_TITLE_TXT, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindTitle->SetFont(font);
	toolbar->AddControl( m_FindTitle );

#ifdef FB_SEARCH_COMBO_CTRL
	toolbar->AddTool(ID_TITLE_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR), _("Find book by title"));
	toolbar->AddSeparator();
#endif // __WXMSW__

	toolbar->AddTool(ID_MODE_TREE, _("Hierarchy"), wxArtProvider::GetBitmap(fbART_REPORT_VIEW, wxART_TOOLBAR), _("Hierarchy of authors and series"));
	toolbar->AddTool(ID_MODE_LIST, _("List"), wxArtProvider::GetBitmap(fbART_LIST_VIEW, wxART_TOOLBAR), _("Simple list"));

#ifdef FB_SEARCH_COMBO_CTRL
	toolbar->AddSeparator();
#endif // __WXMSW__

	toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR), _("Export to external device"));

	toolbar->SetFont(font);
	toolbar->Realize();

	return toolbar;
}

#endif // __WXMAC__

void FbMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void FbMainFrame::OnNewZip( wxCommandEvent& event )
{
	wxString mask;
	mask << _("Digital books and archives") << (wxString)wxT(" (*.fb2; *.zip)|*.zip;*.Zip;*.ZIP;*.fb2;*.Fb2;*.FB2;*.epub;*.Epub;*.EPUB");
	mask << wxT('|') << _("Digital books in FB2") << (wxString)wxT(" (*.fb2)|*.fb2;*.Fb2;*.FB2");
	mask << wxT('|') << _("Digital books in EPUB") << (wxString)wxT(" (*.epub)|*.epub;*.Epub;*.EPUB");
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
		FbProgressDlg scaner(dlg.GetParent(), _("Add file"));
		scaner.RunThread(new FbZipImportThread(&scaner, paths));
		scaner.ShowModal();
	}
}

void FbMainFrame::OnFolder( wxCommandEvent& event )
{
	FbDirImportDlg::Execute(this);
}

void FbMainFrame::OnProgressUpdate(wxUpdateUIEvent& event)
{
	if (m_ProgressBar) {
		m_ProgressBar->SetProgress(event.GetInt());
		m_ProgressBar->SetStatusText(event.GetText(), 0);
		m_ProgressBar->SetStatusText(event.GetString(), 2);
	}
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
		if (!show && FbParams(FB_CLEAR_LOG)) {
			m_LogCtrl->AssignModel(new FbLogModel);
		}
		info->Show(show);
		m_FrameManager.Update();
	}
}

void FbMainFrame::OnHideLog(wxCommandEvent& event)
{
	ShowLog();
}

void FbMainFrame::OnFindAuthor(wxCommandEvent& event)
{
	FindAuthor(m_FindAuthor->GetValue());
}

void FbMainFrame::OnFindTitle(wxCommandEvent& event)
{
	FindTitle(m_FindTitle->GetValue(), m_FindAuthor->GetValue());
}

void FbMainFrame::FindTitle(const wxString &title, const wxString &author)
{
	if (!title.IsEmpty()) {
		FbMasterFindInfo info(title, author);
		wxString text = _("Search"); text << COLON << title;
		OpenInfo(info, text, ID_FRAME_FIND);
	}
}

void FbMainFrame::FindAuthor(const wxString &text)
{
	FbFrameAuth * authors = wxDynamicCast(FindFrameById(ID_FRAME_AUTH, true), FbFrameAuth);
	if (!authors) authors = new FbFrameAuth(&m_FrameNotebook, true);
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

void FbMainFrame::OnInitFrame(wxCommandEvent & event)
{
	bool select = event.GetInt();
	wxWindow * frame = CreateFrame(event.GetId(), select);
	if (select && frame) frame->Update();
}

void FbMainFrame::OnMenuFrame(wxCommandEvent & event)
{
	wxWindow * frame = wxGetKeyState(WXK_CONTROL) ? NULL : FindFrameById(event.GetId(), true);
	if (!frame) frame = CreateFrame(event.GetId(), true);
	if (frame) frame->Update();
}

wxWindow * FbMainFrame::CreateFrame(wxWindowID id, bool select)
{
	switch ( id ) {
		case ID_FRAME_AUTH: return new FbFrameAuth(&m_FrameNotebook, select);
		case ID_FRAME_GENR: return new FbFrameGenr(&m_FrameNotebook, select);
		case ID_FRAME_FLDR: return new FbFrameFldr(&m_FrameNotebook, select);
		case ID_FRAME_DOWN: return new FbFrameDown(&m_FrameNotebook, select);
		case ID_FRAME_SEQN: return new FbFrameSeqn(&m_FrameNotebook, select);
		case ID_FRAME_DATE: return new FbFrameDate(&m_FrameNotebook, select);
		default: return NULL;
	}
}

wxWindow * FbMainFrame::FindFrameById(const int id, bool bActivate)
{
	int sel = m_FrameNotebook.GetSelection();
	if (sel != wxNOT_FOUND && m_FrameNotebook.GetPage(sel)->GetId() == id) {
		return m_FrameNotebook.GetPage(sel);
	}

	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		if (m_FrameNotebook.GetPage(i)->GetId() == id) {
			wxWindow * result = m_FrameNotebook.GetPage(i);
			if (bActivate) m_FrameNotebook.SetSelection(i);
			return result;
		}
	}

	return NULL;
}

void FbMainFrame::OnMenuNothing(wxCommandEvent& event)
{
	wxMessageBox(_("This function is not available yet in this version."));
}

void FbMainFrame::OnReadOnlyUpdate(wxUpdateUIEvent& event)
{
	event.Enable(!FbCollection::IsReadOnly());
}

void FbMainFrame::OnDatabaseInfo(wxCommandEvent & event)
{
	FbFrameInfo::Execute(this);
}

void FbMainFrame::OnVacuum(wxCommandEvent & event)
{
	wxString msg = _("Update full-text search index?");
	int answer = wxMessageBox(msg, MyRuLib::ProgramName(), wxOK | wxCANCEL);
	if (answer == wxOK) {
		FbProgressDlg dlg(this, _("Processing collection"));
		dlg.RunThread(new FbFulltextThread(&dlg));
		dlg.ShowModal();
	}
}

void FbMainFrame::OnUpdate(wxCommandEvent & event)
{
	FbUpdateThread * thread = new FbUpdateThread;
	thread->Execute();
}

void FbMainFrame::OnUpdateUpdate(wxUpdateUIEvent& event)
{
	int code = FbParams(DB_DATAFILE_DATE);
	event.Enable(code && code < FbDateTime::Today().Code() + 20000000);
}

void FbMainFrame::OnUpdateFolder(FbFolderEvent & event)
{
	if (event.m_type == FT_DOWNLOAD) {
		if (FbParams(FB_AUTO_DOWNLD)) wxGetApp().StartDownload();
		FbFrameDown * frame = wxDynamicCast(FindFrameById(ID_FRAME_DOWN, false), FbFrameDown);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);

		size_t count = m_FrameNotebook.GetPageCount();
		for (size_t i = 0; i < count; ++i) {
			FbFrameBase * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameBase);
			if (frame) frame->RefreshBooks();
		}
	} else {
		FbFrameFldr * frame = wxDynamicCast(FindFrameById(ID_FRAME_FLDR, false), FbFrameFldr);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);
	}
}

void FbMainFrame::OnUpdateMaster(FbMasterEvent & event)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameBase * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameBase);
		if (frame) frame->UpdateMaster(event);
	}
}

void FbMainFrame::OnOpenAuth(FbOpenEvent & event)
{
	FbMasterAuthInfo info(event.m_code);
	wxString title = FbCollection::GetAuth(event.m_code, 0);
	OpenInfo(info, title, ID_FRAME_NODE);
}

void FbMainFrame::OnOpenSeqn(FbOpenEvent & event)
{
	FbMasterSeqnInfo info(event.m_code);
	wxString title = FbCollection::GetSeqn(event.m_code, 0);
	OpenInfo(info, title, ID_FRAME_NODE);
}

void FbMainFrame::OpenInfo(const FbMasterInfo & info, const wxString & title, wxWindowID winid)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameFind * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameFind);
		if (frame && frame->GetInfo() == info) {
			m_FrameNotebook.SetSelection(i);
			return;
		}
	}
	new FbFrameFind(&m_FrameNotebook, winid, info, FbString(title).Shorten());
}

void FbMainFrame::OpenClss(int code, bool select)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameClss * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameClss);
		if (frame && frame->GetCode() == code) {
			m_FrameNotebook.SetSelection(i);
			return;
		}
	}
	FbFrameClss::Create(&m_FrameNotebook, code, select);
}

void FbMainFrame::OnInfoCommand(wxCommandEvent & event)
{
	FbFrameInfo * frame = wxDynamicCast(FindFrameById(ID_FRAME_INFO, true), FbFrameInfo);
	if (!frame) frame = new FbFrameInfo(&m_FrameNotebook, true);
	frame->Load(event.GetString());
}

void FbMainFrame::OnProgress(FbProgressEvent & event)
{
	if (m_ProgressBar) {
		m_ProgressBar->SetProgress(event.m_pos);
		m_ProgressBar->SetStatusText(event.m_str, 0);
		m_ProgressBar->SetStatusText(event.m_text, 2);
	}
}

void FbMainFrame::UpdateBook(int id)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameBase * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameBase);
		if (frame) frame->UpdateInfo(id);
	}
}

void FbMainFrame::OnUpdateBook(wxCommandEvent & event)
{
	UpdateBook(event.GetInt());
}

void FbMainFrame::OnDatabaseOpen(wxCommandEvent & event)
{
	wxString filename = FbDataOpenDlg::Execute(this);
	if (!filename.IsEmpty()) OpenDatabase(filename);
}

void FbMainFrame::OnUpdateFonts(wxCommandEvent & event)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
/*
		FbAuiMDIChildFrame * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbAuiMDIChildFrame);
		if (frame) frame->UpdateFonts();
*/
	}
}

void FbMainFrame::OnMenuRecent(wxCommandEvent & event)
{
	int param = event.GetId() - wxID_FILE + FB_RECENT_0;
	wxString filename = FbParams(param);
	if (filename.IsEmpty()) return;

	if (wxFileName::FileExists(filename)) {
		OpenDatabase(filename);
	} else {
		wxLogWarning(_("File not found: ") + filename);
		FbParams(param) = wxEmptyString;
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
		wxString filename = FbParams(i + FB_RECENT_0);
		if (filename.IsEmpty()) continue;
		wxString fileinfo = FbParams(i + FB_TITLE_0);
		submenu->Append(wxID_FILE + i, filename, fileinfo);
	}
}

void FbMainFrame::OpenDatabase(const wxString &filename)
{
	if (wxGetApp().OpenDatabase(filename)) {
		SetTitle(GetTitle());
		SaveFrameList();
		while (m_FrameNotebook.GetPageCount()) {
			m_FrameNotebook.DeletePage(0);
		}
		RestoreFrameList();
		UpdateMenuRefs();
	}
}

void FbMainFrame::OnFullScreen(wxCommandEvent& event)
{
	bool show = !IsFullScreen();
	long style = wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;
	ShowFullScreen(show, style);
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
	int index = m_FrameNotebook.GetSelection();
	if (index != wxNOT_FOUND) {
		m_FrameNotebook.DeletePage(index);
	}
}

void FbMainFrame::OnWindowCloseAll(wxCommandEvent & event)
{
	while (m_FrameNotebook.GetPageCount()) {
		m_FrameNotebook.DeletePage(0);
	}
}

void FbMainFrame::OnWindowNext(wxCommandEvent & event)
{
	int count = (int) m_FrameNotebook.GetPageCount();
	if (count == 0) return;
	int index = m_FrameNotebook.GetSelection() + 1;
	if (index >= count) index = 0;
	m_FrameNotebook.SetSelection(index);
}

void FbMainFrame::OnWindowPrev(wxCommandEvent & event)
{
	int index = m_FrameNotebook.GetSelection() - 1;
	if (index < 0) {
		index = m_FrameNotebook.GetPageCount() - 1;
		if (index < 0) return;
	}
	m_FrameNotebook.SetSelection(index);
}

void FbMainFrame::Localize(int language)
{
}

void FbMainFrame::OnDatabaseGenres(wxCommandEvent & event)
{
	(new FbGenreThread)->Execute();
}

void FbMainFrame::OnAllowNotebookDnD(wxAuiNotebookEvent& event)
{
	event.Allow();
}

void FbMainFrame::OnNotebookChanged(wxAuiNotebookEvent& event)
{
	wxMenuBar * menubar = GetMenuBar();
	if (!menubar) return;

	wxWindow * window = FbMainFrame::GetActiveChild();
	bool enable = window && wxIsKindOf(window, FbFrameBase);

#ifdef FB_INCLUDE_READER
	wxMenu * menu = m_MenuTree;
	wxString title = _("&Books");
	if (window && window->GetId() == ID_FRAME_READ) {
		if (!m_MenuRead) m_MenuBook = m_MenuRead = new FbMenuBar::MenuRead();
		menu = m_MenuRead;
		title = _("&Book");
		enable = true;
	}
	if (m_MenuBook == menu) {
		m_MenuBook = menubar->Replace(fbBOOK_MENU_POSITION, menu, title);
	}
#endif // FB_INCLUDE_READER

	menubar->EnableTop(fbBOOK_MENU_POSITION, enable);
}

void FbMainFrame::OnNotebookClosed(wxAuiNotebookEvent& event)
{
	wxMenuBar * menubar = GetMenuBar();
	if (!menubar) return;

	if (m_FrameNotebook.GetPageCount() == 0) {
		menubar->EnableTop(fbBOOK_MENU_POSITION, false);
	}
}

wxWindow * FbMainFrame::GetActiveChild()
{
	int index = m_FrameNotebook.GetSelection();
	return index == wxNOT_FOUND ? NULL : m_FrameNotebook.GetPage(index);
}

void FbMainFrame::OnIdle( wxIdleEvent & event)
{
	FbFrameBase * child = wxDynamicCast(GetActiveChild(), FbFrameBase);
	int count = child ? child->GetBookCount(): 0;
	wxString msg;
	if (count) {
		msg = wxString::Format(wxT(" %d "), count);
		msg << wxPLURAL("book", "books", count);
	}
	if (m_ProgressBar) m_ProgressBar->SetStatusText(msg, 2);

	FbLogModel * model = wxDynamicCast(m_LogCtrl->GetModel(), FbLogModel);
	if (model && model->Update()) {
		ShowLog(true);
		m_LogCtrl->Refresh();
	}
}

void FbMainFrame::OnFoundNothing(wxCommandEvent & event)
{
	wxString msg = wxString::Format(_("Nothing was found on pattern \"%s\""), event.GetString().c_str());
	wxMessageBox(msg, _("Searching"));
	int index = m_FrameNotebook.GetPageIndex((wxWindow*)event.GetEventObject());
	if (index != wxNOT_FOUND) m_FrameNotebook.DeletePage(index);
}

void FbMainFrame::OnNavigationKey(wxNavigationKeyEvent& event)
{
	event.Skip();
}

void FbMainFrame::OnPaneClose(wxAuiManagerEvent& event)
{
	if (event.pane->name == wxT("Log")) {
		m_LogCtrl->AssignModel(new FbLogModel);
	}
}

void FbMainFrame::OnStatusBar(wxCommandEvent & event)
{
	if (!m_ProgressBar) CreateStatusBar(false);
	bool show = !m_ProgressBar->IsShown();
	FbParams(FB_STATUS_SHOW) = show;
	m_ProgressBar->Show(show);
	PositionStatusBar();
	Layout();
}

void FbMainFrame::OnStatusBarUpdate(wxUpdateUIEvent  & event)
{
	event.Enable(!IsFullScreen());
	event.Check(m_ProgressBar && m_ProgressBar->IsShown());
}

void FbMainFrame::OnImportFinish(wxCommandEvent& event)
{
	size_t count = m_FrameNotebook.GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbFrameBase * frame = wxDynamicCast(m_FrameNotebook.GetPage(i), FbFrameBase);
		if (frame) frame->UpdateMaster();
	}
}
