/***************************************************************
 * Name:      MyRuLibMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#include <wx/artprov.h>
#include <wx/splitter.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/dcclient.h>
#include "FbConst.h"
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "FbManager.h"
#include "FbParams.h"
#include "SettingsDlg.h"
#include "ImpThread.h"
#include "FbFrameSearch.h"
#include "FbFrameFavorites.h"
#include "VacuumThread.h"

BEGIN_EVENT_TABLE(MyRuLibMainFrame, wxAuiMDIParentFrame)
    EVT_TOOL(wxID_NEW, MyRuLibMainFrame::OnNewZip)
    EVT_MENU(wxID_OPEN, MyRuLibMainFrame::OnFolder)
    EVT_MENU(wxID_EXIT, MyRuLibMainFrame::OnExit)
	EVT_MENU(wxID_PREFERENCES, MyRuLibMainFrame::OnSetup)
    EVT_MENU(ID_MENU_SEARCH, MyRuLibMainFrame::OnMenuSearch)
    EVT_MENU(ID_MENU_AUTHOR, MyRuLibMainFrame::OnMenuAuthor)
    EVT_MENU(ID_MENU_TITLE, MyRuLibMainFrame::OnMenuTitle)
    EVT_MENU(ID_FIND_AUTHOR, MyRuLibMainFrame::OnFindAuthor)
    EVT_MENU(ID_MENU_DB_INFO, MyRuLibMainFrame::OnDatabaseInfo)
    EVT_MENU(ID_MENU_VACUUM, MyRuLibMainFrame::OnVacuum)
	EVT_TEXT_ENTER(ID_FIND_AUTHOR, MyRuLibMainFrame::OnFindAuthorEnter)
    EVT_MENU(ID_FIND_TITLE, MyRuLibMainFrame::OnFindTitle)
	EVT_TEXT_ENTER(ID_FIND_TITLE, MyRuLibMainFrame::OnFindTitleEnter)
	EVT_MENU(ID_OPEN_WEB, MyRuLibMainFrame::OnOpenWeb)
	EVT_MENU(wxID_ABOUT, MyRuLibMainFrame::OnAbout)
    EVT_UPDATE_UI(ID_PROGRESS_START, MyRuLibMainFrame::OnProgressStart)
    EVT_UPDATE_UI(ID_PROGRESS_UPDATE, MyRuLibMainFrame::OnProgressUpdate)
    EVT_UPDATE_UI(ID_PROGRESS_FINISH, MyRuLibMainFrame::OnProgressFinish)
    EVT_MENU(ID_FB2_ONLY, MyRuLibMainFrame::OnChangeFilter)
    EVT_MENU(ID_ERROR, MyRuLibMainFrame::OnError)
    EVT_MENU(ID_LOG_TEXTCTRL, MyRuLibMainFrame::OnHideLog)
    EVT_AUI_PANE_CLOSE(MyRuLibMainFrame::OnPanelClosed)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MyRuLibMainFrame::OnNotebookPageClose)
END_EVENT_TABLE()

MyRuLibMainFrame::MyRuLibMainFrame()
{
	Create(NULL, wxID_ANY, wxT("MyRuLib - My Russian Library"));
}

MyRuLibMainFrame::~MyRuLibMainFrame()
{
	m_FrameManager.UnInit();
}

bool MyRuLibMainFrame::Create(wxWindow * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIParentFrame::Create(parent, id, title, wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU);
	if(res)	{
		CreateControls();
        #if defined(__WXMSW__)
		wxIcon icon(wxT("aaaa"));
		SetIcon(icon);
        #else
		SetIcon(wxArtProvider::GetIcon(wxART_FRAME_ICON));
        #endif
	}
	return res;
}

void MyRuLibMainFrame::CreateControls()
{
	SetMenuBar(CreateMenuBar());

	const int widths[] = {-92, -57, -35, -22};
    m_ProgressBar.Create(this, ID_PROGRESSBAR);
    m_ProgressBar.SetFieldsCount(4);
	m_ProgressBar.SetStatusWidths(4, widths);
	SetStatusBar(&m_ProgressBar);

	m_LOGTextCtrl.Create(this, ID_LOG_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER|wxTE_DONTWRAP);
	FbFrameAuthor * authors = new FbFrameAuthor(this, ID_FRAME_AUTHORS, wxT("Авторы"));
	authors->SelectRandomLetter();

	GetNotebook()->SetWindowStyleFlag(
        wxAUI_NB_TOP|
		wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
		wxNO_BORDER);
	GetNotebook()->SetSelection(0);

	m_FrameManager.SetManagedWindow(this);

	m_FrameManager.AddPane(CreateToolBar(), wxAuiPaneInfo().Name(wxT("ToolBar")).Top().Show(true).ToolbarPane().Dockable(false).PaneBorder(false));
	m_FrameManager.AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
	m_FrameManager.AddPane(&m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Log")).Caption(_("Информационные сообщения")).Show(false));
	m_FrameManager.Update();

	m_FindAuthor.SetFocus();

	Centre();
}

void MyRuLibMainFrame::OnSetup(wxCommandEvent & event)
{
    SettingsDlg::Execute(this);
}

void MyRuLibMainFrame::OnOpenWeb(wxCommandEvent & event)
{
    wxLaunchDefaultBrowser(strHomePage);
}

void MyRuLibMainFrame::OnAbout(wxCommandEvent & event)
{
    wxMessageBox(strVersionInfo + wxT("\n\nDatabase:\n") + wxGetApp().GetAppData());
}

wxAuiToolBar * MyRuLibMainFrame::CreateToolBar()
{
	wxAuiToolBar * toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	wxAuiToolBarArt * art = new wxAuiDefaultToolBarArt;
	art->SetElementSize(wxAUI_TBART_GRIPPER_SIZE, 0);
	toolbar->SetArtProvider(art);

    wxWindowDC dc(toolbar);
    int text_width = 0, text_height = 0;
    dc.GetTextExtent(wxT("Автор:"), &text_width, &text_height);

	toolbar->AddTool(wxID_NEW, _("Импорт файла"), wxArtProvider::GetBitmap(wxART_NEW), _("Добавить в библиотеку новые файлы"));
	toolbar->AddTool(wxID_OPEN, _("Импорт папки"), wxArtProvider::GetBitmap(wxART_FILE_OPEN), _("Добавить в библиотеку директорию"));
	toolbar->AddSeparator();
	toolbar->AddLabel(wxID_ANY, _("Автор:"), text_width);
	m_FindAuthor.Create(toolbar, ID_FIND_AUTHOR, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	toolbar->AddControl( &m_FindAuthor );
	toolbar->AddTool(ID_FIND_AUTHOR, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск автора"));
	toolbar->AddSeparator();
	toolbar->AddLabel(wxID_ANY, _("Книга:"), text_width);
	m_FindTitle.Create(toolbar, ID_FIND_TITLE, wxEmptyString, wxDefaultPosition, wxSize(180, -1), wxTE_PROCESS_ENTER);
	toolbar->AddControl( &m_FindTitle );
	toolbar->AddTool(ID_FIND_TITLE, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск книги по заголовку"));
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
//	toolbar->AddSeparator();
//	toolbar->AddTool(ID_FB2_ONLY, _("Фильтр"), wxArtProvider::GetBitmap(wxART_HELP_BOOK), _("Только файлы Fb2"), wxITEM_CHECK);
//	toolbar->ToggleTool(ID_FB2_ONLY, FbParams().GetValue(FB_FB2_ONLY) );
	toolbar->Realize();

	return toolbar;
}

wxMenuBar * MyRuLibMainFrame::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	wxMenuItem * tempItem;
	wxMenu * menu;

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_NEW, _("Добавить файл"))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_OPEN, _("Добавить директорию"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menu->AppendSeparator();
	menu->Append(wxID_EXIT, _("Выход\tAlt+F4"))->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Файл"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(ID_MENU_SEARCH, _("Расширенный"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND));
	menu->AppendSeparator();
	menu->Append(ID_MENU_AUTHOR, _("по Автору"));
	menu->Append(ID_MENU_TITLE, _("по Заголовку"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Поиск"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	menu->Append(ID_MENU_VACUUM, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Сервис"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->Append(wxID_ABOUT, _("О программе…"))->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_PAGE));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&?"));

	return menuBar;
}

void MyRuLibMainFrame::OnChangeFilter(wxCommandEvent& event)
{
    /*
    FbParams().SetValue(FB_FB2_ONLY, toolbar-> GetToolState(ID_FB2_ONLY));

    RecordIDClientData * data = (RecordIDClientData *)
        m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
    if(data) {
        FbManager::FillBooks(m_BooksListView, data->GetID(), toolbar-> GetToolState(ID_FB2_ONLY));
        m_BooksInfoPanel->SetPage(wxEmptyString);
    }
    */
}

void MyRuLibMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void MyRuLibMainFrame::OnNewZip( wxCommandEvent& event ){

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

        ZipImportThread *thread = new ZipImportThread(paths);
        thread->m_info = _("Обработка файла:");
        if ( thread->Create() != wxTHREAD_NO_ERROR ) {
            wxLogError(wxT("Can't create thread!"));
            return;
        }
        thread->Run();
	}
}

void MyRuLibMainFrame::OnFolder( wxCommandEvent& event ) {

    wxDirDialog dlg (
		this,
		_("Выберите папку для рекурсивного импорта файлов в библиотеку…"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST,
		wxDefaultPosition
    );

	if (dlg.ShowModal() == wxID_OK) {
        DirImportThread *thread = new DirImportThread(dlg.GetPath());
        thread->m_info = wxT("Обработка папки:");

        if ( thread->Create() != wxTHREAD_NO_ERROR ) {
            wxLogError(wxT("Can't create thread!"));
            return;
        }
        thread->Run();
	}
}

void MyRuLibMainFrame::OnProgressStart(wxUpdateUIEvent& event)
{
	m_ProgressBar.SetRange(event.GetInt());
	m_ProgressBar.SetStatusText(event.GetText(), 0);
	m_ProgressBar.SetStatusText(wxEmptyString, 2);
}

void MyRuLibMainFrame::OnProgressUpdate(wxUpdateUIEvent& event)
{
	m_ProgressBar.SetProgress(event.GetInt());
	m_ProgressBar.SetStatusText(event.GetText(), 0);
	m_ProgressBar.SetStatusText(event.GetString(), 2);
}

void MyRuLibMainFrame::OnProgressFinish(wxUpdateUIEvent& event)
{
	m_StatusText = wxEmptyString;
	m_ProgressBar.SetProgress(0);
	m_ProgressBar.SetStatusText(wxEmptyString, 0);
	m_ProgressBar.SetStatusText(wxEmptyString, 2);
}

void MyRuLibMainFrame::OnError(wxCommandEvent& event)
{
    m_LOGTextCtrl.AppendText(event.GetString() + wxT("\n"));
    TogglePaneVisibility(wxT("Log"), true);
}

void MyRuLibMainFrame::TogglePaneVisibility(const wxString &pane_name, bool show)
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i) {
		if(all_panes.Item(i).name == pane_name) {
		    if (all_panes.Item(i).IsShown() != show) {
                all_panes.Item(i).Show(show);
                m_FrameManager.Update();
		    }
            break;
		}
	}
}

void MyRuLibMainFrame::OnPanelClosed(wxAuiManagerEvent& event)
{
    if (event.pane->name == wxT("Log")) {
        m_LOGTextCtrl.Clear();
    }
}

void MyRuLibMainFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
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

void MyRuLibMainFrame::OnHideLog(wxCommandEvent& event)
{
    TogglePaneVisibility(wxT("Log"), false);
}

void MyRuLibMainFrame::OnFindTitle(wxCommandEvent & event)
{
    FindTitle(m_FindTitle.GetValue());
}

void MyRuLibMainFrame::OnFindTitleEnter(wxCommandEvent& event)
{
    FindTitle(event.GetString());
}

void MyRuLibMainFrame::FindTitle(const wxString &text)
{
    if ( text.IsEmpty() ) return;

    wxLogInfo(_("Search title: %s"), text.c_str());

    wxString caption = wxString::Format(_("Поиск: «%s»"), text.c_str());
	FbFrameSearch * frame = new FbFrameSearch(this, wxID_ANY, caption);
	frame->Update();
	frame->DoSearch(text);
}

void MyRuLibMainFrame::OnFindAuthor(wxCommandEvent& event)
{
    FindAuthor(m_FindAuthor.GetValue());
}

void MyRuLibMainFrame::OnFindAuthorEnter(wxCommandEvent& event)
{
    FindAuthor(event.GetString());
}

void MyRuLibMainFrame::FindAuthor(const wxString &text)
{
    wxLogInfo(_("Search author: %s"), text.c_str());

    FbFrameAuthor * authors = NULL;
	size_t count = GetNotebook()->GetPageCount();
	for (size_t i = 0; i < count; ++i) {
        if (GetNotebook()->GetPage(i)->GetId() == ID_FRAME_AUTHORS) {
            authors = (FbFrameAuthor*) GetNotebook()->GetPage(i);
            GetNotebook()->SetSelection(i);
            break;
		}
	}
	if (!authors) {
	    authors = new FbFrameAuthor(this, ID_FRAME_AUTHORS, _("Авторы"));
        GetNotebook()->SetSelection( GetNotebook()->GetPageCount() - 1 );
        authors->Update();
	}

    if ( text.IsEmpty() )
        authors->SelectRandomLetter();
    else
        authors->FindAuthor(text);

    authors->ActivateAuthors();

}

void MyRuLibMainFrame::OnMenuAuthor(wxCommandEvent& event)
{
	wxString text = wxGetTextFromUser(_("Введите шаблон для поиска:"), _("Поиск по автору"));
	if (text.IsEmpty()) return;
	FindAuthor(text);
}

void MyRuLibMainFrame::OnMenuTitle(wxCommandEvent& event)
{
	wxString text = wxGetTextFromUser(_("Введите шаблон для поиска:"), _("Поиск по заголовку"));
	if (text.IsEmpty()) return;
	FindTitle(text);
}

void MyRuLibMainFrame::OnMenuSearch(wxCommandEvent& event)
{
    wxMessageBox(_("Функционал расширенного поиска\nне реализован в данной версии."));
}

void MyRuLibMainFrame::OnDatabaseInfo(wxCommandEvent & event)
{
    wxMessageBox(_("Функционал не реализован в данной версии."));
}

void MyRuLibMainFrame::OnVacuum(wxCommandEvent & event)
{
    wxString msg = _("Выполнить реструктуризацию базы данных?");
    if (wxMessageBox(msg, _("Подтверждение"), wxOK | wxCANCEL, this) != wxOK) return;

    VacuumThread::Execute();
}
