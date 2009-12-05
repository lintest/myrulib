/***************************************************************
 * Name:	  MyRuLibMain.cpp
 * Purpose:   Code for Application Frame
 * Author:	  Kandrashin Denis (mail@lintest.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:   GPL
 **************************************************************/

#include "FbMainFrame.h"
#include <wx/splitter.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>
#include <wx/artprov.h>
#include "FbConst.h"
#include "MyRuLibApp.h"
#include "FbManager.h"
#include "SettingsDlg.h"
#include "FbImportThread.h"
#include "FbDataOpenDlg.h"
#include "FbFrameSearch.h"
#include "FbFrameDate.h"
#include "FbFrameGenres.h"
#include "FbFrameFolder.h"
#include "FbFrameDownld.h"
#include "FbFrameSequen.h"
#include "FbFrameInfo.h"
#include "FbMainMenu.h"
#include "FbConfigDlg.h"
#include "FbDownloader.h"
#include "FbUpdateThread.h"
#include "InfoCash.h"
#include "FbAboutDlg.h"

BEGIN_EVENT_TABLE(FbMainFrame, wxAuiMDIParentFrame)
	EVT_TOOL(wxID_NEW, FbMainFrame::OnNewZip)
	EVT_MENU(wxID_OPEN, FbMainFrame::OnFolder)
	EVT_MENU(wxID_EXIT, FbMainFrame::OnExit)
	EVT_MENU(ID_MENU_SEARCH, FbMainFrame::OnMenuTitle)
	EVT_MENU(ID_FRAME_AUTHOR, FbMainFrame::OnMenuAuthor)
	EVT_MENU(ID_FRAME_GENRES, FbMainFrame::OnMenuGenres)
	EVT_MENU(ID_FRAME_FOLDER, FbMainFrame::OnMenuFolder)
	EVT_MENU(ID_FRAME_DOWNLD, FbMainFrame::OnMenuDownld)
	EVT_MENU(ID_FRAME_SEQUEN, FbMainFrame::OnMenuSequen)
	EVT_MENU(ID_FRAME_ARCH, FbMainFrame::OnMenuNothing)
	EVT_MENU(ID_FRAME_DATE, FbMainFrame::OnMenuCalendar)
	EVT_MENU(ID_MENU_DB_INFO, FbMainFrame::OnDatabaseInfo)
	EVT_MENU(ID_MENU_DB_OPEN, FbMainFrame::OnDatabaseOpen)
	EVT_MENU(ID_MENU_VACUUM, FbMainFrame::OnVacuum)
	EVT_MENU(ID_MENU_CONFIG, FbMainFrame::OnMenuConfig)
	EVT_MENU(wxID_PREFERENCES, FbMainFrame::OnSetup)
	EVT_MENU(ID_OPEN_WEB, FbMainFrame::OnOpenWeb)
	EVT_MENU(wxID_ABOUT, FbMainFrame::OnAbout)

	EVT_MENU(ID_FIND_AUTHOR, FbMainFrame::OnFindAuthor)
	EVT_TEXT_ENTER(ID_FIND_AUTHOR, FbMainFrame::OnFindAuthorEnter)
	EVT_MENU(ID_FIND_TITLE, FbMainFrame::OnFindTitle)
	EVT_TEXT_ENTER(ID_FIND_TITLE, FbMainFrame::OnFindTitleEnter)

	EVT_MENU(ID_RECENT_1, FbMainFrame::OnMenuRecent)
	EVT_MENU(ID_RECENT_2, FbMainFrame::OnMenuRecent)
	EVT_MENU(ID_RECENT_3, FbMainFrame::OnMenuRecent)
	EVT_MENU(ID_RECENT_4, FbMainFrame::OnMenuRecent)
	EVT_MENU(ID_RECENT_5, FbMainFrame::OnMenuRecent)
	EVT_UPDATE_UI(ID_RECENT_1, FbMainFrame::OnRecentUpdate)
	EVT_UPDATE_UI(ID_RECENT_2, FbMainFrame::OnRecentUpdate)
	EVT_UPDATE_UI(ID_RECENT_3, FbMainFrame::OnRecentUpdate)
	EVT_UPDATE_UI(ID_RECENT_4, FbMainFrame::OnRecentUpdate)
	EVT_UPDATE_UI(ID_RECENT_5, FbMainFrame::OnRecentUpdate)

	EVT_UPDATE_UI(ID_PROGRESS_UPDATE, FbMainFrame::OnProgressUpdate)

	EVT_MENU(ID_ERROR, FbMainFrame::OnError)
	EVT_MENU(ID_LOG_TEXTCTRL, FbMainFrame::OnHideLog)
	EVT_MENU(ID_UPDATE_FONTS, FbMainFrame::OnUpdateFonts)

	EVT_AUI_PANE_CLOSE(FbMainFrame::OnPanelClosed)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, FbMainFrame::OnNotebookPageClose)

	EVT_FB_OPEN(ID_BOOK_AUTHOR, FbMainFrame::OnOpenAuthor)
	EVT_FB_FOLDER(ID_UPDATE_FOLDER, FbMainFrame::OnUpdateFolder)
	EVT_FB_PROGRESS(ID_PROGRESS_UPDATE, FbMainFrame::OnProgress)
	EVT_COMMAND(ID_DATABASE_INFO, fbEVT_BOOK_ACTION, FbMainFrame::OnInfoCommand)
	EVT_COMMAND(ID_UPDATE_BOOK, fbEVT_BOOK_ACTION, FbMainFrame::OnUpdateBook)
END_EVENT_TABLE()

wxString FbMainFrame::GetTitle() const
{
	return FbParams::GetText(DB_LIBRARY_TITLE) + wxT(" - ") + strProgramName;
}

FbMainFrame::FbMainFrame()
{
	Create(NULL, wxID_ANY, GetTitle());
}

FbMainFrame::~FbMainFrame()
{
	FbParams params;
	wxSize size = GetSize();
	params.SetValue(FB_FRAME_MAXIMIZE, IsMaximized());
	params.SetValue(FB_FRAME_WIDTH, size.x);
	params.SetValue(FB_FRAME_HEIGHT, size.y);

	m_FrameManager.UnInit();
}

bool FbMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
	wxSize size;
	bool maximized = FbParams::GetValue(FB_FRAME_MAXIMIZE);
	if (maximized) {
		size = wxSize( FbParams::DefaultValue(FB_FRAME_WIDTH), FbParams::DefaultValue(FB_FRAME_HEIGHT) );
	} else {
		size = wxSize( FbParams::GetValue(FB_FRAME_WIDTH), FbParams::GetValue(FB_FRAME_HEIGHT) );
	}

	bool res = wxAuiMDIParentFrame::Create(parent, id, title, wxDefaultPosition, size, wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU);
	if(res)	{
		if (maximized) Maximize();
		CreateControls();
		LoadIcon();
		if (maximized) {
			size.x = GetBestSize().x;
			SetSize(size);
		}
	}
	return res;
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
	SetMenuBar(new FbMainMenu);

	const int widths[] = {-90, -50, -50, -10};
	m_ProgressBar.Create(this, ID_PROGRESSBAR);
	m_ProgressBar.SetFieldsCount(4);
	m_ProgressBar.SetStatusWidths(4, widths);
	SetStatusBar(&m_ProgressBar);

	m_LOGTextCtrl.Create(this, ID_LOG_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER|wxTE_DONTWRAP);

	GetNotebook()->SetWindowStyleFlag(
		wxAUI_NB_TOP|
		wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
		wxNO_BORDER);
	GetNotebook()->SetSelection(0);

	m_FrameManager.SetManagedWindow(this);

	m_ToolBar = CreateToolBar();

	m_FrameManager.AddPane(m_ToolBar, wxAuiPaneInfo().Name(wxT("ToolBar")).Top().Show(true).ToolbarPane().Dockable(false).PaneBorder(false));
	m_FrameManager.AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
	m_FrameManager.AddPane(&m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Log")).Caption(_("Информационные сообщения")).Show(false));
	m_FrameManager.Update();

	m_FindAuthor.SetFocus();

	Layout();
	Centre();

	FbCommandEvent(wxEVT_COMMAND_MENU_SELECTED, ID_FRAME_AUTHOR).Post(this);
}

void FbMainFrame::OnSetup(wxCommandEvent & event)
{
	SettingsDlg::Execute(this);
}

void FbMainFrame::OnMenuConfig(wxCommandEvent& event)
{
	FbConfigDlg::Execute(this);
	SetTitle(GetTitle());
}

void FbMainFrame::OnOpenWeb(wxCommandEvent & event)
{
	wxLaunchDefaultBrowser(strHomePage);
}

void FbMainFrame::OnAbout(wxCommandEvent & event)
{
	FbAboutDlg about(this);
	about.ShowModal();
}

wxAuiToolBar * FbMainFrame::CreateToolBar()
{
	wxAuiToolBar * toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);

	wxFont font = FbParams::GetFont(FB_FONT_TOOL);

	wxAuiToolBarArt * art = new wxAuiDefaultToolBarArt;
	art->SetElementSize(wxAUI_TBART_GRIPPER_SIZE, 0);
	toolbar->SetArtProvider(art);

	wxWindowDC dc(toolbar);
	int text_width = 0, text_height = 0;
	dc.SetFont(font);
	dc.GetTextExtent(wxT("Автор:"), &text_width, &text_height);

	toolbar->AddTool(wxID_NEW, _("Импорт файла"), wxArtProvider::GetBitmap(wxART_NEW), _("Добавить в библиотеку новые файлы"));
	toolbar->AddTool(wxID_OPEN, _("Импорт папки"), wxArtProvider::GetBitmap(wxART_FILE_OPEN), _("Добавить в библиотеку директорию"));
	toolbar->AddSeparator();
	toolbar->AddLabel(wxID_ANY, _("Автор:"), text_width);
	m_FindAuthor.Create(toolbar, ID_FIND_AUTHOR, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindAuthor.SetFont(font);
	toolbar->AddControl( &m_FindAuthor );
	toolbar->AddTool(ID_FIND_AUTHOR, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск автора"));
	toolbar->AddSeparator();
	toolbar->AddLabel(wxID_ANY, _("Книга:"), text_width);
	m_FindTitle.Create(toolbar, ID_FIND_TITLE, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	m_FindTitle.SetFont(font);
	toolbar->AddControl( &m_FindTitle );
	toolbar->AddTool(ID_FIND_TITLE, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск книги по заголовку"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_MODE_TREE, _("Иерархия"), wxArtProvider::GetBitmap(wxART_LIST_VIEW), _("Иерархия авторов и серий"));
	toolbar->AddTool(ID_MODE_LIST, _("Список"), wxArtProvider::GetBitmap(wxART_REPORT_VIEW), _("Простой список"));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	toolbar->Realize();

	toolbar->SetFont(font);

	return toolbar;
}

void FbMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void FbMainFrame::OnNewZip( wxCommandEvent& event )
{
	wxFileDialog dlg (
		this,
		_("Выберите zip-файл для добавления в библиотеку…"),
		wxEmptyString,
		wxEmptyString,
		_("Электронные книги и архивы (*.fb2; *.zip)|*.zip;*.Zip;*.ZIP;*.fb2;*.Fb2;*.FB2|Электронные книги FB2 (*.fb2)|*.fb2;*.Fb2;*.FB2|Архивы ZIP (*.zip)|*.zip;*.Zip;*.ZIP|Все файлы (*.*)|*.*"),
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths(paths);

		FbImportThread *thread = new FbZipImportThread(paths);
		thread->m_info = _("Обработка файла:");
		if ( thread->Create() != wxTHREAD_NO_ERROR ) {
			wxLogError(wxT("Can't create thread!"));
			return;
		}
		thread->Run();
	}
}

void FbMainFrame::OnFolder( wxCommandEvent& event ) {

	wxDirDialog dlg (
		this,
		_("Выберите папку для рекурсивного импорта файлов в библиотеку…"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
		FbImportThread *thread = new FbDirImportThread(dlg.GetPath());
		thread->m_info = wxT("Обработка папки:");

		if ( thread->Create() != wxTHREAD_NO_ERROR ) {
			wxLogError(wxT("Can't create thread!"));
			return;
		}
		thread->Run();
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
	ShowPane(wxT("Log"));
	m_LOGTextCtrl.AppendText(event.GetString() + wxT("\n"));
}

void FbMainFrame::TogglePaneVisibility(const wxString &pane_name, bool show)
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i) {
		if(all_panes.Item(i).name == pane_name) {
			bool show = ! all_panes.Item(i).IsShown();
			all_panes.Item(i).Show(show);
			m_FrameManager.Update();
			break;
		}
	}
}

void FbMainFrame::ShowPane(const wxString &pane_name)
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i) {
		if(all_panes.Item(i).name == pane_name) {
			if (!all_panes.Item(i).IsShown()) {
				all_panes.Item(i).Show(true);
				m_FrameManager.Update();
			}
			break;
		}
	}
}

void FbMainFrame::OnPanelClosed(wxAuiManagerEvent& event)
{
/*
	if (event.pane->name == wxT("Log")) {
		m_LOGTextCtrl.Clear();
	}
*/
}

void FbMainFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(FbFrameAuthor)))
	{
		int res = wxMessageBox(wxT("Are you sure you want to close/hide this notebook page?"),
					   wxT("wxAUI"),
					   wxYES_NO,
					   this);
		if (res != wxYES)
			evt.Veto();
	}
}

void FbMainFrame::OnHideLog(wxCommandEvent& event)
{
	TogglePaneVisibility(wxT("Log"), false);
}

void FbMainFrame::OnFindTitle(wxCommandEvent & event)
{
	FindTitle(m_FindTitle.GetValue(), m_FindAuthor.GetValue());
}

void FbMainFrame::OnFindTitleEnter(wxCommandEvent& event)
{
	FindTitle(event.GetString(), m_FindAuthor.GetValue());
}

void FbMainFrame::FindTitle(const wxString &title, const wxString &author)
{
	FbFrameSearch::Execute(this, title, author);
}

void FbMainFrame::OnFindAuthor(wxCommandEvent& event)
{
	FindAuthor(m_FindAuthor.GetValue());
}

void FbMainFrame::OnFindAuthorEnter(wxCommandEvent& event)
{
	FindAuthor(event.GetString());
}

void FbMainFrame::FindAuthor(const wxString &text)
{
	FbFrameAuthor * authors = wxDynamicCast(FindFrameById(ID_FRAME_AUTHOR, true), FbFrameAuthor);
	if (!authors) {
		authors = new FbFrameAuthor(this);
		if ( text.IsEmpty() ) authors->SelectRandomLetter();
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		authors->Update();
	}
	if ( !text.IsEmpty() ) authors->FindAuthor(text);
	authors->ActivateAuthors();
}

void FbMainFrame::OnMenuAuthor(wxCommandEvent& event)
{
	FindAuthor(wxEmptyString);
}

void FbMainFrame::OnMenuTitle(wxCommandEvent& event)
{
	wxString text = wxGetTextFromUser(_("Введите строку для поиска:"), _("Поиск по заголовку"));
	if (text.IsEmpty()) return;
	FindTitle(text, wxEmptyString);
}

void FbMainFrame::OnMenuGenres(wxCommandEvent & event)
{
	FbFrameGenres * frame = wxDynamicCast(FindFrameById(ID_FRAME_GENRES, true), FbFrameGenres);
	if (!frame) {
		frame = new FbFrameGenres(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
}

void FbMainFrame::OnMenuFolder(wxCommandEvent & event)
{
	FbFrameFolder * frame = wxDynamicCast(FindFrameById(ID_FRAME_FOLDER, true), FbFrameFolder);
	if (!frame) {
		frame = new FbFrameFolder(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
}

void FbMainFrame::OnMenuDownld(wxCommandEvent & event)
{
	FbFrameDownld * frame = wxDynamicCast(FindFrameById(ID_FRAME_DOWNLD, true), FbFrameDownld);
	if (!frame) {
		frame = new FbFrameDownld(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
}

void FbMainFrame::OnMenuSequen(wxCommandEvent & event)
{
	FbFrameSequen * frame = wxDynamicCast(FindFrameById(ID_FRAME_DOWNLD, true), FbFrameSequen);
	if (!frame) {
		frame = new FbFrameSequen(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
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
	wxMessageBox(_("Функционал не реализован в данной версии."));
}

void FbMainFrame::OnDatabaseInfo(wxCommandEvent & event)
{
	FbFrameInfo::Execute();
}

void FbMainFrame::OnVacuum(wxCommandEvent & event)
{
	wxString msg = _("Выполнить реструктуризацию базы данных?");
	if (wxMessageBox(msg, _("Подтверждение"), wxOK | wxCANCEL, this) == wxOK)
		(new FbUpdateThread(strUpdateAuthorCount, wxT("VACUUM")))->Execute();
}

void FbMainFrame::OnUpdateFolder(FbFolderEvent & event)
{
	if (event.m_type == FT_DOWNLOAD) {
		if (FbParams::GetValue(FB_AUTO_DOWNLD)) FbDownloader::Start();
		FbFrameDownld * frame = wxDynamicCast(FindFrameById(ID_FRAME_DOWNLD, false), FbFrameDownld);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);
	} else {
		FbFrameFolder * frame = wxDynamicCast(FindFrameById(ID_FRAME_FOLDER, false), FbFrameFolder);
		if (frame) frame->UpdateFolder(event.m_folder, event.m_type);
	}
}

void FbMainFrame::OnOpenAuthor(FbOpenEvent & event)
{
	FbFrameAuthor * frame = wxDynamicCast(FindFrameById(ID_FRAME_AUTHOR, true), FbFrameAuthor);
	if (!frame) {
		frame = new FbFrameAuthor(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
	frame->OpenAuthor(event.m_author, event.m_book);
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
	FbDataOpenDlg dlg(this);
	if (dlg.ShowModal() != wxID_OK) return;
	OpenDatabase(dlg.GetFilename());
}

void FbMainFrame::OnUpdateFonts(wxCommandEvent & event)
{
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
		FbAuiMDIChildFrame * frame = wxDynamicCast(GetNotebook()->GetPage(i), FbAuiMDIChildFrame);
		frame->UpdateFonts();
	}
}

void FbMainFrame::OnMenuRecent(wxCommandEvent & event)
{
	int param = event.GetId() - ID_RECENT_0 + FB_RECENT_0;
	wxString filename = FbParams::GetText(param);
	if (filename.IsEmpty()) return;

	if (wxFileName::FileExists(filename)) {
		OpenDatabase(filename);
	} else {
		wxLogWarning(wxT("File not found: ") + filename);
		FbParams().SetText(param, wxEmptyString);
	}
}

void FbMainFrame::OnRecentUpdate(wxUpdateUIEvent& event)
{
	int param = event.GetId() - ID_RECENT_0 + FB_RECENT_0;
	wxString filename = FbParams::GetText(param);
	event.SetText(filename);
	event.Show(!filename.IsEmpty());
}

void FbMainFrame::OpenDatabase(const wxString &filename)
{
	if (wxGetApp().OpenDatabase(filename)) {
		SetTitle(GetTitle());
		InfoCash::Empty();
		while (GetNotebook()->GetPageCount()) delete GetNotebook()->GetPage(0);
		FindAuthor(wxEmptyString);
	}
}

void FbMainFrame::SetStatus(const wxString &text)
{
	m_ProgressBar.SetStatusText(text, 2);
}

void FbMainFrame::OnMenuCalendar(wxCommandEvent & event)
{
	OnMenuNothing(event);
/*
	FbFrameDate * frame = wxDynamicCast(FindFrameById(ID_FRAME_DATE, true), FbFrameDate);
	if (!frame) {
		frame = new FbFrameDate(this);
		GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
		frame->Update();
	}
*/
}

