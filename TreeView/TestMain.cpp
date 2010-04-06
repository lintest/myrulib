/***************************************************************
 * Name:      DataViewMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "TestMain.h"
#include "TestMain.h"
#include "TestApp.h"
#include "FbTreeView.h"
#include "FbTreeModel.h"
#include <wx/srchctrl.h>
#include "FbSearchCtrl.h"

BEGIN_EVENT_TABLE( DataViewFrame, wxFrame )
	EVT_CLOSE( DataViewFrame::OnClose )
	EVT_MENU( idMenuQuit, DataViewFrame::OnQuit )
	EVT_MENU( idMenuAbout, DataViewFrame::OnAbout )
    EVT_SEARCHCTRL_SEARCH_BTN(idSearchBtn, DataViewFrame::OnSearchBtn)
	EVT_TEXT_ENTER(idSearchBtn, DataViewFrame::OnSearchBtn)
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

	m_dataview = new FbTreeViewCtrl( this, idDataView, wxDefaultPosition, wxDefaultSize);
	m_dataview->AddColumn(0, _("title"), 200);
	m_dataview->AddColumn(1, _("author"), 150);
	m_dataview->AddColumn(2, _("type"), 50);
	m_dataview->AddColumn(3, _("size"), 50);
	m_dataview->AssignModel(new FbTreeModelList(500));
	m_dataview->SetFocus();

	m_dataview->SetSortedColumn(2);

	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );

	{
		wxBoxSizer* bSizerDir = new wxBoxSizer( wxHORIZONTAL );

		wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Wine temp folder:"), wxDefaultPosition, wxDefaultSize, 0 );
		info->Wrap( -1 );
		bSizerDir->Add( info, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

		wxTextCtrl * edit = new wxTextCtrl( this, idSearchBtn, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
		edit->SetMinSize( wxSize( 200,-1 ) );
		bSizerDir->Add( edit, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		wxTextCtrl * text = new wxTextCtrl( this, idSearchBtn, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		text->SetMinSize( wxSize( 200,-1 ) );
		bSizerDir->Add( text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		bSizer1->Add( bSizerDir, 0, wxEXPAND, 5 );
	}

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

void DataViewFrame::OnSearchBtn(wxCommandEvent& event)
{
    wxLogMessage(wxT("DataViewFrame::OnSearchBtn"));
}

