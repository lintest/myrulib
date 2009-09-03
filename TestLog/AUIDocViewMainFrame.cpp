#include "AUIDocViewMainFrame.h"
#include <wx/aboutdlg.h>

IMPLEMENT_DYNAMIC_CLASS(AUIDocViewMainFrame, wxAuiMDIParentFrame);

BEGIN_EVENT_TABLE(AUIDocViewMainFrame , wxAuiMDIParentFrame)
EVT_MENU(wxID_EXIT, AUIDocViewMainFrame::OnExit)
EVT_MENU(wxID_ABOUT, AUIDocViewMainFrame::OnAbout)
EVT_MENU(ID_TOGGLE_LOGWINDOW, AUIDocViewMainFrame::OnToggleLogWindow)
EVT_UPDATE_UI(ID_TOGGLE_LOGWINDOW, AUIDocViewMainFrame::OnToggleLogWindowUpdateUI)
END_EVENT_TABLE()

AUIDocViewMainFrame::AUIDocViewMainFrame ()
{
}

AUIDocViewMainFrame::AUIDocViewMainFrame (wxWindow * parent, wxWindowID id,const wxString & title)
{
	Create(parent, id, title);
}

AUIDocViewMainFrame::~AUIDocViewMainFrame()
{
	m_FrameManager.UnInit();
}

bool AUIDocViewMainFrame::Create(wxWindow * parent, wxWindowID id,const wxString & title)
{
	bool res = wxAuiMDIParentFrame::Create(parent, id, title, wxDefaultPosition,
		wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxFRAME_NO_WINDOW_MENU);

	if(res) CreateControls();

	return res;
}

wxMenuBar * AUIDocViewMainFrame::CreateMainMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;

	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(wxID_EXIT, _("Exit\tAlt+F4"));
	menuBar->Append(fileMenu, _("File"));

	wxMenu * viewMenu = new wxMenu;
	viewMenu->AppendCheckItem(ID_TOGGLE_LOGWINDOW, _("Toggle Help Window"));
	menuBar->Append(viewMenu, _("View"));

	wxMenu * helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, _("About..."));
	menuBar->Append(helpMenu, _("Help"));

	return menuBar;
}

void AUIDocViewMainFrame::CreateControls()
{
	SetMenuBar(AUIDocViewMainFrame::CreateMainMenuBar());
	CreateStatusBar(2);
	m_LOGTextCtrl = new wxTextCtrl(this, ID_LOG_TEXTCTRL, wxEmptyString,
		wxDefaultPosition, wxSize(250, 100),
		wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER);

	GetNotebook()->SetWindowStyleFlag(wxAUI_NB_TOP|
		wxAUI_NB_TAB_MOVE |
		wxAUI_NB_SCROLL_BUTTONS |
		wxNO_BORDER);
	GetNotebook()->SetSelection(0);

	m_FrameManager.SetManagedWindow(this);
	m_FrameManager.AddPane(GetNotebook(), wxAuiPaneInfo().
		Name(wxT("CenterPane")).CenterPane());
	m_FrameManager.AddPane(m_LOGTextCtrl, wxAuiPaneInfo().Bottom().
		Name(wxT("Help")).Caption(_("Help Window")));
	m_FrameManager.Update();
}

void AUIDocViewMainFrame::OnExit(wxCommandEvent & event)
{
	Close();
}

void AUIDocViewMainFrame::OnAbout(wxCommandEvent & event)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("MultiView Test"));
	info.SetVersion(wxT("v0.1"));
	info.SetWebSite(wxT("http://wxwidgets.info"));
	info.AddDeveloper(wxT("Volodymir (T-Rex) Tryapichko"));
	wxAboutBox(info);
}

wxTextCtrl * AUIDocViewMainFrame::GetLOGTextCtrl()
{
	return m_LOGTextCtrl;
}

void AUIDocViewMainFrame::OnToggleLogWindow(wxCommandEvent & event)
{
	TogglePaneVisibility(wxT("Help"));
}

void AUIDocViewMainFrame::OnToggleLogWindowUpdateUI(wxUpdateUIEvent & event)
{
	event.Check(GetPaneVisibility(wxT("Help")));
}

bool AUIDocViewMainFrame::GetPaneVisibility(wxString pane_name)
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t i, count;
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
	{
		if(all_panes.Item(i).name == pane_name)
		{
				return all_panes.Item(i).IsShown();
		}
	}
	return false;
}

void AUIDocViewMainFrame::TogglePaneVisibility(wxString pane_name)
{
	wxAuiPaneInfoArray& all_panes = m_FrameManager.GetAllPanes();
	size_t i, count;
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
	{
		if(all_panes.Item(i).name == pane_name)
		{
			all_panes.Item(i).Show(!all_panes.Item(i).IsShown());
			m_FrameManager.Update();
			break;
		}
	}
}
