/***************************************************************
 * Name:      DataViewMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "TestMain.h"
#include "TestApp.h"
#include "FbTreeView.h"

BEGIN_EVENT_TABLE( DataViewFrame, wxFrame )
	EVT_CLOSE( DataViewFrame::OnClose )
	EVT_MENU( idMenuQuit, DataViewFrame::OnQuit )
	EVT_MENU( idMenuAbout, DataViewFrame::OnAbout )
END_EVENT_TABLE()

DataViewFrame::DataViewFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxMenuBar * mbar = new wxMenuBar( 0 );
	wxMenu * fileMenu = new wxMenu();

	wxMenuItem* menuFileOpen = new wxMenuItem( fileMenu, idOpenList, wxString( _("&Open list") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );

	wxMenuItem* menuOpenTree = new wxMenuItem( fileMenu, idOpenTree, wxString( _("&Open tree") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuOpenTree );

	wxMenuItem* menuFileQuit = new wxMenuItem( fileMenu, idMenuQuit, wxString( _("&Quit") ) + wxT('\t') + wxT("Alt+F4"), _("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );

	mbar->Append( fileMenu, _("&File") );

	wxMenu * helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( _("&About") ) + wxT('\t') + wxT("F1"), _("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );

	mbar->Append( helpMenu, _("&Help") );

	this->SetMenuBar( mbar );

	m_statusbar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_dataview = new wxTreeListCtrl( this, idDataView, wxDefaultPosition, wxDefaultSize, 0);
//	m_dataview->AddColumn(_("title"));
//	m_dataview->AddColumn(_("author"));

	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

    wxLogWindow * log = new wxLogWindow(this, _("Log Messages"), false);
    log->GetFrame()->Move(GetPosition().x + GetSize().x + 10, GetPosition().y);
    log->Show();

    m_statusbar->SetStatusText(_("Hello Code::Blocks user!"), 0);
}

DataViewFrame::~DataViewFrame()
{
}

void DataViewFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void DataViewFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void DataViewFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("About"), _("Welcome to..."));
}
