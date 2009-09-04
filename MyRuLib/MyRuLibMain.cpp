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
#include "FbConst.h"
#include "MyRuLibMain.h"
#include "MyRuLibApp.h"
#include "FbManager.h"
#include "FbParams.h"
#include "SettingsDlg.h"
#include "ImpThread.h"
#include "FbFrameAuthor.h"
#include "FbFrameSearch.h"

BEGIN_EVENT_TABLE(MyRuLibMainFrame, wxAuiMDIParentFrame)
    EVT_MENU(wxID_EXIT, MyRuLibMainFrame::OnExit)
	EVT_MENU(wxID_PREFERENCES, MyRuLibMainFrame::OnSetup)
	EVT_MENU(wxID_ABOUT, MyRuLibMainFrame::OnAbout)
    EVT_TOOL(wxID_FIND, MyRuLibMainFrame::OnFind)
    EVT_TOOL(wxID_NEW, MyRuLibMainFrame::OnNewZip)
    EVT_MENU(wxID_OPEN, MyRuLibMainFrame::OnFolder)
    EVT_MENU(ID_PROGRESS_START, MyRuLibMainFrame::OnProgressStart)
    EVT_MENU(ID_PROGRESS_UPDATE, MyRuLibMainFrame::OnProgressUpdate)
    EVT_MENU(ID_PROGRESS_FINISH, MyRuLibMainFrame::OnProgressFinish)
    EVT_MENU(ID_FB2_ONLY, MyRuLibMainFrame::OnChangeFilter)
    EVT_MENU(ID_ERROR, MyRuLibMainFrame::OnError)
    EVT_MENU(ID_LOG_TEXTCTRL, MyRuLibMainFrame::OnHideLog)
    EVT_AUI_PANE_CLOSE(MyRuLibMainFrame::OnPanelClosed)
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
	SetMenuBar(new wxMenuBar);
	SetToolBar(CreateButtonBar());

	const int widths[] = {-92, -57, -35, -22};
    m_ProgressBar.Create(this, ID_PROGRESSBAR);
    m_ProgressBar.SetFieldsCount(4);
	m_ProgressBar.SetStatusWidths(4, widths);
	SetStatusBar(&m_ProgressBar);

	m_LOGTextCtrl.Create(this, ID_LOG_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER|wxTE_DONTWRAP);
	new FbFrameAuthor(this, wxID_ANY, wxT("Авторы"));
	new FbFrameSearch(this, wxID_ANY, wxT("Поиск"));
	new FbFrameSearch(this, wxID_ANY, wxT("Избранное"));

	GetNotebook()->SetWindowStyleFlag(
        wxAUI_NB_TOP|
        wxAUI_NB_TAB_FIXED_WIDTH |
		wxAUI_NB_SCROLL_BUTTONS |
		wxNO_BORDER);
	GetNotebook()->SetSelection(0);

	m_FrameManager.SetManagedWindow(this);
	m_FrameManager.AddPane(GetNotebook(), wxAuiPaneInfo().Name(wxT("CenterPane")).CenterPane());
	m_FrameManager.AddPane(&m_LOGTextCtrl, wxAuiPaneInfo().Bottom().Name(wxT("Log")).Caption(_("Информационные сообщения")).Show(false));
	m_FrameManager.Update();

	Centre();
}

void MyRuLibMainFrame::OnSetup(wxCommandEvent & event)
{
    SettingsDlg::Execute(this);
}

void MyRuLibMainFrame::OnAbout(wxCommandEvent & event)
{
    wxMessageBox(strVersionInfo + wxT("\n\nDatabase:\n") + wxGetApp().GetAppData());
}

wxToolBar * MyRuLibMainFrame::CreateButtonBar()
{
	m_ToolBar.Create(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORZ_TEXT);
	m_ToolBar.AddTool(wxID_NEW, _("Импорт"), wxArtProvider::GetBitmap(wxART_NEW), _("Добавить в библиотеку новые файлы ZIP"));
	m_ToolBar.AddSeparator();
	m_FindTextCtrl.Create(&m_ToolBar, ID_FIND_TEXT, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);
	m_ToolBar.AddControl( &m_FindTextCtrl );
	m_ToolBar.AddTool(wxID_FIND, _("Найти"), wxArtProvider::GetBitmap(wxART_FIND), _("Поиск по подстроке"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(wxID_SAVE, _("Экспорт"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Запись на внешнее устройство"));
	m_ToolBar.AddSeparator();
	m_ToolBar.AddTool(ID_FB2_ONLY, _("Фильтр"), wxArtProvider::GetBitmap(wxART_HELP_BOOK), _("Только файлы Fb2"), wxITEM_CHECK);
	m_ToolBar.ToggleTool(ID_FB2_ONLY, FbParams().GetValue(FB_FB2_ONLY) );
	m_ToolBar.Realize();

	return &m_ToolBar;
}

void MyRuLibMainFrame::OnChangeFilter(wxCommandEvent& event)
{
    /*
    FbParams().SetValue(FB_FB2_ONLY, m_ToolBar-> GetToolState(ID_FB2_ONLY));

    RecordIDClientData * data = (RecordIDClientData *)
        m_AuthorsListBox->GetClientObject(m_AuthorsListBox->GetSelection());
    if(data) {
        FbManager::FillBooks(m_BooksListView, data->GetID(), m_ToolBar-> GetToolState(ID_FB2_ONLY));
        m_BooksInfoPanel->SetPage(wxEmptyString);
    }
    */
}

void MyRuLibMainFrame::OnExit(wxCommandEvent & event)
{
	wxUnusedVar(event);
	Close();
}

void MyRuLibMainFrame::OnFind( wxCommandEvent& event )
{
    wxCommandEvent subEvent( wxEVT_COMMAND_TEXT_ENTER, ID_FIND_TEXT );
    subEvent.SetString(m_FindTextCtrl.GetValue());
    wxPostEvent(this, subEvent);
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
        thread->m_info = wxT("Обработка файла:");
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

void MyRuLibMainFrame::OnProgressStart(wxCommandEvent& event)
{
	m_StatusText = event.GetString();
	m_ProgressBar.SetRange(event.GetInt());
	m_ProgressBar.SetStatusText(m_StatusText, 0);
	m_ProgressBar.SetStatusText(wxEmptyString, 2);
}

void MyRuLibMainFrame::OnProgressUpdate(wxCommandEvent& event)
{
	m_ProgressBar.SetProgress(event.GetInt());
	m_ProgressBar.SetStatusText(m_StatusText, 0);
	m_ProgressBar.SetStatusText(event.GetString(), 2);
}

void MyRuLibMainFrame::OnProgressFinish(wxCommandEvent& event)
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

void MyRuLibMainFrame::OnHideLog(wxCommandEvent& event)
{
    TogglePaneVisibility(wxT("Log"), false);
}
