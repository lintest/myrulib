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
	EVT_MENU( idOpenList, GUIFrame::_wxFB_OnOpenList )
	EVT_MENU( idOpenTree, GUIFrame::_wxFB_OnOpenTree )
	EVT_MENU( idMenuQuit, GUIFrame::_wxFB_OnQuit )
	EVT_MENU( idMenuAbout, GUIFrame::_wxFB_OnAbout )
	EVT_DATAVIEW_ITEM_ACTIVATED(idDataView, GUIFrame::OnActivated)
END_EVENT_TABLE()

GUIFrame::GUIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
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

	m_dataview = new wxDataViewCtrl( this, idDataView, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE | wxDV_ROW_LINES);

	int flags = wxDATAVIEW_COL_RESIZABLE | wxCOL_SORTABLE | wxCOL_REORDERABLE;
/*
	FbTitleRenderer *cr = new FbTitleRenderer;
	wxDataViewColumn *column = new wxDataViewColumn("title", cr, FbBookModel::COL_TITLE, 200, wxALIGN_LEFT, flags );
	m_dataview->AppendColumn( column );
*/
	m_dataview->AppendTextColumn(_("author"), FbBookModel::COL_AUTHOR, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_LEFT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
	m_dataview->AppendTextColumn(_("rowid"),  FbBookModel::COL_ROWID,  wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
	m_dataview->AppendTextColumn(_("book"),   FbBookModel::COL_BOOKID, wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
	m_dataview->AppendTextColumn(_("size"),   FbBookModel::COL_SIZE,   wxDATAVIEW_CELL_ACTIVATABLE, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);


	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
}

GUIFrame::~GUIFrame()
{
}

void GUIFrame::OnActivated(wxDataViewEvent& event)
{
	m_dataview->Expand(event.GetItem());
	wxMessageBox("Activated");
}
