/***************************************************************
 * Name:      MyRuLibMain.h
 * Purpose:   Defines Application Frame
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-05-05
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef MYRULIBMAIN_H
#define MYRULIBMAIN_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/aui/tabmdi.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include "ProgressBar.h"
#include "LimitedTextCtrl.h"

class MyRuLibMainFrame: public wxAuiMDIParentFrame
{
public:
	MyRuLibMainFrame();
	virtual ~MyRuLibMainFrame();
	wxString GetFindText() { return m_FindTextCtrl.GetValue(); };
private:
	bool Create(wxWindow * parent, wxWindowID id, const wxString & title);
	void CreateControls();
    void TogglePaneVisibility(const wxString &pane_name, bool show);
	wxToolBar * CreateButtonBar();
private:
	wxToolBar m_ToolBar;
	wxTextCtrl m_FindTextCtrl;
	ProgressBar m_ProgressBar;
	wxString m_StatusText;
	wxAuiManager m_FrameManager;
	LimitedTextCtrl m_LOGTextCtrl;
private:
	void OnExit(wxCommandEvent & event);
    void OnSetup(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnFind( wxCommandEvent& event );
	void OnNewZip( wxCommandEvent& event );
	void OnRegZip( wxCommandEvent& event );
	void OnFolder( wxCommandEvent& event );
	void OnProgressStart(wxCommandEvent& event);
	void OnProgressUpdate(wxCommandEvent& event);
	void OnProgressFinish(wxCommandEvent& event);
    void OnChangeFilter(wxCommandEvent& event);
    void OnError(wxCommandEvent& event);
    void OnHideLog(wxCommandEvent& event);
    void OnPanelClosed(wxAuiManagerEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // MYRULIBMAIN_H
