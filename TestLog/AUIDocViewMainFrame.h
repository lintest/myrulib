#ifndef _AUI_DOCVIEW_MAINFRAME_H
#define _AUI_DOCVIEW_MAINFRAME_H

#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/aui/aui.h>
#include <wx/aui/tabmdi.h>

#define AUIDocViewMainFrameTitle wxT("AUI DocView Test")

enum HeaderStyle
{
	HS_DEFAULT,
	HS_SIMPLE
};

enum
{
	ID_LOG_TEXTCTRL = 10001,
	ID_HEADER_DEFAULT_STYLE,
	ID_HEADER_SIMPLE_STYLE,
	ID_TOGGLE_LOGWINDOW
};

class AUIDocViewMainFrame : public wxAuiMDIParentFrame
{		
	DECLARE_DYNAMIC_CLASS(AUIDocViewMainFrame);
	HeaderStyle m_HeaderStyle;
	wxTextCtrl * m_LOGTextCtrl;
	wxAuiManager m_FrameManager;
	void CreateControls();	
	static wxMenuBar * CreateMainMenuBar();
public:
	AUIDocViewMainFrame();
	AUIDocViewMainFrame(wxWindow * parent, wxWindowID id = wxID_ANY, 
		const wxString & title = AUIDocViewMainFrameTitle);
	~AUIDocViewMainFrame();
	bool Create(wxWindow * parent, wxWindowID id = wxID_ANY, 
		const wxString & title = AUIDocViewMainFrameTitle);

	wxTextCtrl * GetLOGTextCtrl();
	HeaderStyle GetHeaderStyle();

	bool GetPaneVisibility(wxString pane_name);
	void TogglePaneVisibility(wxString pane_name);

	DECLARE_EVENT_TABLE()
	void OnExit(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnToggleLogWindow(wxCommandEvent & event);
	void OnSwitchHeaderStyle(wxCommandEvent & event);
	void OnSwitchHeaderStyleUpdateUI(wxUpdateUIEvent & event);
	void OnToggleLogWindowUpdateUI(wxUpdateUIEvent & event);
};

#endif
