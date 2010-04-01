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
#include "FbDataView.h"
#include "FbModelData.h"
#include "FbListModel.h"
#include "FbTreeModel.h"

BEGIN_EVENT_TABLE( DataViewFrame, wxFrame )
	EVT_CLOSE( DataViewFrame::OnClose )
	EVT_MENU( idOpenList, DataViewFrame::OnOpenList )
	EVT_MENU( idOpenTree, DataViewFrame::OnOpenTree )
	EVT_MENU( idMenuQuit, DataViewFrame::OnQuit )
	EVT_MENU( idMenuAbout, DataViewFrame::OnAbout )
	EVT_DATAVIEW_ITEM_ACTIVATED(idDataView, DataViewFrame::OnActivated)
	EVT_CHAR(DataViewFrame::OnKeyUp)
END_EVENT_TABLE()

DataViewFrame::DataViewFrame( wxWindow* parent, wxWindowID id )
    : wxFrame( parent, id, wxT("Test wxDataViewCtrl"), wxDefaultPosition, wxSize(600,400) )
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

	m_dataview = new FbDataViewCtrl( this, idDataView, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE | wxDV_ROW_LINES | wxDV_VERT_RULES);

    int flags = wxDATAVIEW_COL_RESIZABLE;

    FbTitleRenderer *cr = new FbTitleRenderer;
    wxDataViewColumn *column = new wxDataViewColumn(wxT("title"), cr, 0, 200, wxALIGN_LEFT, flags );
    m_dataview->AppendColumn( column );

	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

    wxLogWindow * log = new wxLogWindow(this, "Log Messages", false);
    log->GetFrame()->Move(GetPosition().x + GetSize().x + 10, GetPosition().y);
    log->Show();

    m_statusbar->SetStatusText(_("Hello Code::Blocks user!"), 0);
}

DataViewFrame::~DataViewFrame()
{
}

void DataViewFrame::OnActivated(wxDataViewEvent& event)
{
	m_dataview->Expand(event.GetItem());
    wxLogMessage(wxT("DataViewFrame::Activate"));
    event.Skip();
}

void DataViewFrame::OnKeyUp(wxKeyEvent& event)
{
    wxLogMessage(wxT("DataViewFrame::OnChar"));
    event.Skip();
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
    wxMessageBox(_("About..."), _("Welcome to..."));
}

void DataViewFrame::OnOpenList(wxCommandEvent &event)
{
	wxFileDialog dlg (
		this,
		_("Select archive to add to the library"),
		wxEmptyString,
		wxEmptyString,
		wxT("*.db"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
	    FbListModel * model = new FbListModel(dlg.GetPath());
	    m_dataview->AssociateModel(model);

	    while (m_dataview->GetColumnCount()>1) m_dataview->DeleteColumn(m_dataview->GetColumn(1));

        int flags = wxDATAVIEW_COL_RESIZABLE;
        m_dataview->AppendTextColumn(_("author"), 4, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("rowid"),  1, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("book"),   2, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("size"),   3, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        wxDynamicCast(m_dataview, FbDataViewCtrl)->Resize();
	};
}

void DataViewFrame::OnOpenTree(wxCommandEvent &event)
{
	wxFileDialog dlg (
		this,
		_("Select archive to add to the library"),
		wxEmptyString,
		wxEmptyString,
		wxT("*.db"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST,
		wxDefaultPosition
	);

	if (dlg.ShowModal() == wxID_OK) {
        FbTreeModel * model = new FbFullTreeModel(dlg.GetPath());
	    m_dataview->AssociateModel(model);

	    while (m_dataview->GetColumnCount()>1) m_dataview->DeleteColumn(m_dataview->GetColumn(1));

        int flags = wxDATAVIEW_COL_RESIZABLE;
        m_dataview->AppendTextColumn(_("rowid"),  1, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("book"),   2, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        m_dataview->AppendTextColumn(_("size"),   3, wxDATAVIEW_CELL_INERT, 100, wxALIGN_RIGHT, flags)->GetRenderer()->EnableEllipsize(wxELLIPSIZE_NONE);
        wxDynamicCast(m_dataview, FbDataViewCtrl)->Resize();
	};

}

