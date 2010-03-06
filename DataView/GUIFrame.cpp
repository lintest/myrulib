///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "GUIFrame.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( GUIFrame, wxFrame )
	EVT_CLOSE( GUIFrame::_wxFB_OnClose )
	EVT_MENU( idMenuOpen, GUIFrame::_wxFB_OnOpen )
	EVT_MENU( idMenuQuit, GUIFrame::_wxFB_OnQuit )
	EVT_MENU( idMenuAbout, GUIFrame::_wxFB_OnAbout )
END_EVENT_TABLE()

GUIFrame::GUIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxMenuBar * mbar = new wxMenuBar( 0 );
	wxMenu * fileMenu = new wxMenu();

	wxMenuItem* menuFileOpen = new wxMenuItem( fileMenu, idMenuOpen, wxString( _("&Open") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );

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

	m_dataview = new wxDataViewCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_dataview->AppendTextColumn(_("rowid"),  FbBookModel::COL_ROWID, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT);
	m_dataview->AppendTextColumn(_("number"), FbBookModel::COL_NUM, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT);
//	m_dataview->AppendTextColumn(_("title"),  2, wxDATAVIEW_CELL_INERT, 200);

	MyCustomRenderer *cr = new MyCustomRenderer;
	wxDataViewColumn *column = new wxDataViewColumn("custom", cr, FbBookModel::COL_TITLE, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
	m_dataview->AppendColumn( column );

	m_dataview->AppendTextColumn(_("size"),   3, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT);

	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
}

GUIFrame::~GUIFrame()
{
}
