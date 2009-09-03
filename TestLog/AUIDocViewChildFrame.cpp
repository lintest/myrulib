#include "AUIDocViewChildFrame.h"
#include "AUIDocViewMainFrame.h"

#include "news_sticky.xpm"
#include "news_question.xpm"

enum
{
	ID_CHILD_TOGGLE_TOOLBAR = 11001,
	//--- BUTTONS ---
	ID_CHILD_BUTTON_ID_START = 12000
};

AUIDocViewChildFrame::AUIDocViewChildFrame()
{
}

AUIDocViewChildFrame::AUIDocViewChildFrame(wxAuiMDIParentFrame * parent, 
										   wxWindowID id, 
										   const wxString & title, 
										   bool has_toolbar)
{
	m_ToolBar = NULL;	
	Create(parent, id, title, has_toolbar);
}

bool AUIDocViewChildFrame::Create(wxAuiMDIParentFrame * parent, wxWindowID id, 
								  const wxString & title, bool has_toolbar)
{	
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if(res)
	{
		m_HasToolBar = has_toolbar;
		CreateControls();
	}
	return res;
}

void AUIDocViewChildFrame::CreateControls()
{
	SetSizer(new wxBoxSizer(wxVERTICAL));
	Connect(wxID_ANY, wxEVT_ACTIVATE, 
		wxActivateEventHandler(AUIDocViewChildFrame::OnActivated));
	wxToolBar * toolBar = CreateToolBar(wxTB_FLAT|wxTB_NODIVIDER, 
		wxID_ANY, GetTitle());
	if(toolBar)
	{
		GetSizer()->Add(toolBar, 0, wxGROW);
		Connect(ID_CHILD_TOGGLE_TOOLBAR,
				wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(AUIDocViewChildFrame::OnToggleToolbar));
	}
	SetMenuBar(CreateMenuBar());
	SetBackgroundColour(wxColour(100+rand()%156, 
		100+rand()%156, 100+rand()%156));
}

wxMenuBar * AUIDocViewChildFrame::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	
	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(wxID_EXIT, _("Exit\tAlt+F4"));
	menuBar->Append(fileMenu, _("File"));

	wxMenu * viewMenu = new wxMenu;	
	wxMenu * headerMenu = new wxMenu;
	headerMenu->AppendRadioItem(ID_HEADER_DEFAULT_STYLE, _("Default"));
	headerMenu->AppendRadioItem(ID_HEADER_SIMPLE_STYLE, _("Simple"));
	viewMenu->Append(wxID_ANY, _("Header"), headerMenu);
	viewMenu->AppendCheckItem(ID_TOGGLE_LOGWINDOW, _("Toggle Help Window"));
	if(m_HasToolBar)
	{		
		viewMenu->Append(ID_CHILD_TOGGLE_TOOLBAR, _("Toggle toolbar"));			
	}
	menuBar->Append(viewMenu, _("View"));

	wxMenu * helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT, _("About..."));
	menuBar->Append(helpMenu, _("Help"));

	return menuBar;
}

void AUIDocViewChildFrame::OnToggleToolbar(wxCommandEvent & event)
{
	if(m_ToolBar)
	{
		if(m_ToolBar->IsShown())
		{
			GetSizer()->Detach(m_ToolBar);
			m_ToolBar->Hide();
		}
		else
		{
			m_ToolBar->Show();
			GetSizer()->Insert(0, m_ToolBar, 0, wxGROW);
		}
	}
}

wxToolBar * AUIDocViewChildFrame::CreateToolBar(long style,
												wxWindowID winid,
												const wxString& WXUNUSED(name))
{
	if(!m_ToolBar && m_HasToolBar)
	{
		m_ToolBar = new wxToolBar(this, winid, wxDefaultPosition, 
			wxDefaultSize, style);
		wxBitmap bmp;
		for(int i = 0; i < 1+rand()%10; i++)
		{
			if(rand()%2)
			{
				bmp = wxBitmap(news_sticky_xpm);
			} else bmp = wxBitmap(news_question_xpm);
			wxWindowID id = ID_CHILD_BUTTON_ID_START+i;
			m_ToolBar->AddTool(id, bmp, wxString::Format(wxT("Tool %i"), i));
			Connect(id, wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(AUIDocViewChildFrame::OnToolButton));
		}
		m_ToolBar->Realize();
	}
	return m_ToolBar;
}
						
wxToolBar * AUIDocViewChildFrame::GetToolBar() const
{
	return m_ToolBar;
}

void AUIDocViewChildFrame::OnToolButton(wxCommandEvent & event)
{
	wxMessageBox(wxString::Format(_("Button %u pressed"), 
		event.GetId()-ID_CHILD_BUTTON_ID_START+1));
}

void AUIDocViewChildFrame::OnActivated(wxActivateEvent & event)
{	
	AUIDocViewMainFrame * frame = wxDynamicCast(GetMDIParentFrame(), 
		AUIDocViewMainFrame);
	if(frame)
	{
		frame->GetLOGTextCtrl()->SetValue(wxString::Format(
			_("Some help text about '%s'"),	GetTitle().GetData()));
	}
}
