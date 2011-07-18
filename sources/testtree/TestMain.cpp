/***************************************************************
 * Name:      DataViewMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#define FB_LIST_COUNT 20
#define FB_TREE_COUNT 20

#include "TestMain.h"
#include "TestMain.h"
#include "TestApp.h"
#include "FbTreeView.h"
#include "FbTreeModel.h"
#include <wx/listbase.h>

//-----------------------------------------------------------------------------
//  FbTestModelData
//-----------------------------------------------------------------------------

class FbTestModelData: public FbModelData
{
	public:
		FbTestModelData(int i)
			: m_code(i), m_state(false) {}
	public:
		virtual wxString GetValue(FbModel & model, size_t col) const;
	protected:
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
	protected:
		int m_code;
		bool m_state;
		bool m_selected;
		DECLARE_CLASS(FbTestModelData);
};

IMPLEMENT_CLASS(FbTestModelData, FbModelData)

wxString FbTestModelData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case 1:
			return Format(m_code * m_code * 100);
		default:
			return wxString::Format(wxT("Cell (%d, %d)"), m_code, col);
	}
}

//-----------------------------------------------------------------------------
//  FbTreeModelData
//-----------------------------------------------------------------------------

class FbTreeModelData: public FbParentData
{
	public:
		FbTreeModelData(FbModel & model, FbParentData * parent, int i)
			: FbParentData(model, parent), m_code(i), m_state(0), m_selected(false), m_expanded(false) {}
	public:
		virtual wxString GetValue(FbModel & model, size_t col) const;
		virtual int DoGetState(FbModel & model) const
			{ return m_state; }
		virtual void DoSetState(FbModel & model, int state)
			{ m_state = state; }
		virtual bool IsBold(FbModel & model) const
			{ return false; }
		virtual bool IsExpanded(FbModel & model) const
			{ return m_expanded; }
		virtual bool HasChildren(FbModel & model) const
			{ return true; }
		virtual bool Expand(FbModel & model, bool expand);
	protected:
		int m_code;
		int m_state;
		bool m_selected;
		bool m_expanded;
		DECLARE_CLASS(FbTestModelData);
};

IMPLEMENT_CLASS(FbTreeModelData, FbParentData)

wxString FbTreeModelData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case  0: return wxString::Format(wxT("Cell (%d, %d)"), m_code, col); 
		case  1: return Format(m_code * m_code * 100);
		case  2: return wxT("fb2");
		case  3: return wxT("ru");
		case  4: return Format(m_code * m_code * 1000);
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  DataViewFrame
//-----------------------------------------------------------------------------

wxString DataViewFrame::sm_filename;

BEGIN_EVENT_TABLE( DataViewFrame, wxFrame )
	EVT_CLOSE( DataViewFrame::OnClose )
	EVT_MENU( idMenuQuit, DataViewFrame::OnQuit )
	EVT_MENU( idOpenList, DataViewFrame::OnOpenList )
	EVT_MENU( idOpenBook, DataViewFrame::OnOpenBook )
	EVT_MENU( idOpenTree, DataViewFrame::OnOpenTree )
	EVT_MENU( idMenuAbout, DataViewFrame::OnAbout )
	EVT_MENU( idCreateList, DataViewFrame::OnCreateList )
	EVT_MENU( idCreateTree, DataViewFrame::OnCreateTree )
	EVT_TEXT_ENTER(idSearchBtn, DataViewFrame::OnSearchBtn)
	EVT_MENU( ID_APPEND_TYPE, DataViewFrame::OnAppendType )
	EVT_MENU( ID_MODIFY_TYPE, DataViewFrame::OnModifyType )
	EVT_MENU( ID_DELETE_TYPE, DataViewFrame::OnDeleteType )
	EVT_TREE_ITEM_ACTIVATED(ID_TYPE_LIST, DataViewFrame::OnTypeActivated)
	EVT_LIST_COL_CLICK(ID_TYPE_LIST, DataViewFrame::OnColumnClick)
END_EVENT_TABLE()

DataViewFrame::DataViewFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxMenuBar * mbar = new wxMenuBar( 0 );
	wxMenu * fileMenu = new wxMenu();

	wxMenuItem* menuCreateTree = new wxMenuItem( fileMenu, idCreateTree, _("Create test tree"));
	fileMenu->Append( menuCreateTree );

	wxMenuItem* menuCreateList = new wxMenuItem( fileMenu, idCreateList, _("Create test list"));
	fileMenu->Append( menuCreateList );

	fileMenu->AppendSeparator();

	wxMenuItem* menuOpenTree = new wxMenuItem( fileMenu, idOpenTree, wxString( _("&Open book tree") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuOpenTree );

	wxMenuItem* menuOpenBook = new wxMenuItem( fileMenu, idOpenBook, wxString( _("&Open book list") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuOpenBook );

	wxMenuItem* menuFileOpen = new wxMenuItem( fileMenu, idOpenList, wxString( _("&Open author list") ) + wxT('\t') + wxT("Ctrl+O"), _("Open file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );

	fileMenu->AppendSeparator();

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

	wxToolBar * toolbar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORZ_TEXT|wxTB_NODIVIDER|wxTB_NOICONS );
	toolbar->SetToolBitmapSize(wxSize(0,0));
	toolbar->AddTool( ID_APPEND_TYPE, _("Append"), wxNullBitmap);
	toolbar->AddTool( ID_MODIFY_TYPE, _("Modify"), wxNullBitmap);
	toolbar->AddTool( ID_DELETE_TYPE, _("Delete"), wxNullBitmap);
	toolbar->Realize();
	SetToolBar(toolbar);

	long substyle = wxBORDER_SUNKEN | fbTR_VRULES | fbTR_HRULES | fbTR_MULTIPLE | fbTR_DIRECTORY;
	substyle = wxBORDER_SUNKEN | fbTR_VRULES | fbTR_MULTIPLE | fbTR_DIRECTORY;
	m_dataview = new FbTreeViewCtrl( this, ID_TYPE_LIST, wxDefaultPosition, wxDefaultSize, substyle);
	m_dataview->AddColumn(0, _("title"), -10);
	m_dataview->AddColumn(1, _("author"), -10);
	m_dataview->AddColumn(2, _("type"), 3);
	m_dataview->AddColumn(3, _("lang"), -5, wxALIGN_CENTER_HORIZONTAL);
	m_dataview->AddColumn(4, _("size"), 6, wxALIGN_RIGHT);
	m_dataview->AddColumn(4, _("size"), 3, wxALIGN_RIGHT);
	m_dataview->SetFocus();
	m_dataview->SetSortedColumn(2);
	bSizer1->Add( m_dataview, 1, wxEXPAND, 5 );
	CreateTreeModel();

	{
		wxBoxSizer* bSizerDir = new wxBoxSizer( wxHORIZONTAL );

		wxStaticText * info = new wxStaticText( this, wxID_ANY, _("Wine temp folder:"), wxDefaultPosition, wxDefaultSize, 0 );
		info->Wrap( -1 );
		bSizerDir->Add( info, 0, wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

		wxTextCtrl * edit = new wxTextCtrl( this, idSearchBtn, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		edit->SetMinSize( wxSize( 100,-1 ) );
		bSizerDir->Add( edit, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

		wxTextCtrl * text = new wxTextCtrl( this, idSearchBtn, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		text->SetMinSize( wxSize( 100,-1 ) );
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
		sm_filename = dlg.GetPath();
/*
		wxGetApp().OpenCollection(dlg.GetPath());
		FbAuthListInfo info;
		m_thread = new FbAuthListThread(this, info, m_dataview->GetSortedColumn());
		m_thread->Create();
		m_thread->Run();
*/
	};
}

void DataViewFrame::OnOpenBook(wxCommandEvent &event)
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
		sm_filename = dlg.GetPath();
//	    FbModel * model = new FbBookListModel(0);
//		m_dataview->AssignModel(model);
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
		sm_filename = dlg.GetPath();
//	    FbModel * model = new FbBookTreeModel(169);
//		m_dataview->AssignModel(model);
	};
}

void DataViewFrame::OnAppendType(wxCommandEvent& event)
{
}

void DataViewFrame::OnModifyType(wxCommandEvent& event)
{
}

void DataViewFrame::OnDeleteType(wxCommandEvent& event)
{
	m_dataview->Delete();
}

void DataViewFrame::OnTypeActivated(wxTreeEvent & event)
{
	wxLogMessage(wxT("DataViewFrame::OnTypeActivated"));
}

bool FbTreeModelData::Expand(FbModel & model, bool expand) 
{
	if (m_expanded == expand) return false;

	m_expanded = expand;
	if (expand) {
		for (int j = 0; j < FB_TREE_COUNT; j++) {
			new FbTreeModelData(model, this, j);
		}
	} else {
		m_items.Empty();
	}
	return true;
}

void DataViewFrame::CreateTreeModel()
{
	FbTreeModel * model = new FbTreeModel();
	FbTreeModelData * root = new FbTreeModelData(*model, NULL, 0);
	for (int i = 0; i < FB_TREE_COUNT; i++) {
		FbTreeModelData * parent = new FbTreeModelData(*model, root, i);
	}
	model->SetRoot(root);
	m_dataview->AssignModel(model);
}

void DataViewFrame::CreateListModel()
{

	FbListStore * model = new FbListStore;
	for (int i=0; i<FB_LIST_COUNT; i++)
		model->Append(new FbTestModelData(i));
	m_dataview->AssignModel(model);
}

void DataViewFrame::OnCreateList(wxCommandEvent& event)
{
	CreateListModel();
}

void DataViewFrame::OnCreateTree(wxCommandEvent& event)
{
	CreateTreeModel();
}

void DataViewFrame::OnColumnClick(wxListEvent& event)
{
}
