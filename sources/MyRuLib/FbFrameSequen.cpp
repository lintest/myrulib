#include "FbFrameSequen.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbUpdateThread.h"
#include "FbWindow.h"
#include "FbParams.h"
#include "FbSequenDlg.h"

IMPLEMENT_CLASS(FbFrameSequen, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameSequen, FbFrameBase)
    EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameSequen::OnColClick)
	EVT_TEXT_ENTER(ID_SEQUENCE_TXT, FbFrameSequen::OnFindEnter )
	EVT_MENU(ID_SEQUENCE_BTN, FbFrameSequen::OnFindEnter )
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameSequen::OnContextMenu)
	EVT_MENU(ID_MASTER_APPEND, FbFrameSequen::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbFrameSequen::OnMasterModify)
	EVT_MENU(ID_MASTER_DELETE, FbFrameSequen::OnMasterDelete)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameSequen::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameSequen::OnArray)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameSequen::OnBooksCount)
END_EVENT_TABLE()

FbFrameSequen::FbFrameSequen(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_SEQUEN, GetTitle()), m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	CreateControls();
}

void FbFrameSequen::CreateControls()
{
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	m_ToolBar = CreateToolBar();
	sizer->Add( m_ToolBar, 0, wxGROW);

	wxSplitterWindow * splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSP_NOBORDER);
	splitter->SetMinimumPaneSize(50);
	splitter->SetSashGravity(0.33);
	sizer->Add(splitter, 1, wxEXPAND);

	m_MasterList = new FbTreeViewCtrl(splitter, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

	CreateBooksPanel(splitter);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	FindSequence(wxEmptyString);
}

void FbFrameSequen::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Ser."), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

wxToolBar * FbFrameSequen::CreateToolBar(long style, wxWindowID winid, const wxString& name)
{
	wxFont font = FbParams::GetFont(FB_FONT_TOOL);

	wxToolBar * toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, name);
	toolbar->SetFont(font);

	m_FindInfo = new wxStaticText( toolbar, wxID_ANY, (wxString)_("Ser.") + wxT(':'), wxDefaultPosition, wxDefaultSize, 0 );
	m_FindInfo->Wrap( -1 );
	m_FindInfo->SetFont(font);
	toolbar->AddControl( m_FindInfo );

	m_FindText = new wxTextCtrl( toolbar, ID_SEQUENCE_TXT, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );
	m_FindText->SetFont(font);
	toolbar->AddControl( m_FindText );

	toolbar->AddTool(ID_SEQUENCE_BTN, _("Find"), wxArtProvider::GetBitmap(wxART_FIND), _("Find series by name"));
	toolbar->AddTool(wxID_SAVE, _("Export"), wxArtProvider::GetBitmap(wxART_FILE_SAVE), _("Export to external device"));
	toolbar->Realize();
	return toolbar;
}

void FbFrameSequen::CreateMasterThread()
{
	m_MasterList->AssignModel(NULL);
	if (m_MasterThread) m_MasterThread->Wait();
	wxDELETE(m_MasterThread);
	m_MasterThread = new FbSeqnListThread(this, m_info, m_MasterList->GetSortedColumn());
	m_MasterThread->Execute();
}

void FbFrameSequen::FindSequence(const wxString &text)
{
	m_info = text;
	CreateMasterThread();
}

void FbFrameSequen::OpenSequence(const int sequence, const int book)
{
/*
	m_SequenceText = wxEmptyString;
	m_SequenceCode = sequence;
	(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
*/
}

void FbFrameSequen::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameSequen::OnBooksCount(FbCountEvent& event)
{
	if (GetInfo() != event.GetInfo()) return;

	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) {
		model->SetCount(event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

void FbFrameSequen::OnFindEnter(wxCommandEvent& event)
{
	FindSequence(m_FindText->GetValue());
}

void FbFrameSequen::ShowFullScreen(bool show)
{
	m_FindText->Show(!show);
	m_FindInfo->Show(!show);
	if (m_ToolBar) m_ToolBar->Show(!show);
	Layout();
}

FbFrameSequen::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

void FbFrameSequen::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameSequen::ShowContextMenu(const wxPoint& pos, wxTreeItemId)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbSeqnListData * data = wxDynamicCast(&item, FbSeqnListData);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameSequen::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbSequenDlg::Append(newname);
	if (id == 0) return;

	m_MasterList->Append(new FbSeqnListData(id));
}

void FbFrameSequen::OnMasterModify(wxCommandEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model == NULL) return;

	FbModelItem item = m_MasterList->GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbSequenDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	m_MasterList->Replace(new FbSeqnListData(new_id));
}

void FbFrameSequen::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = m_MasterList->GetModel();
	if (model == NULL) return;

	FbModelItem item = model->GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	if (current == NULL) return;

	int id = current->GetCode();

	wxString msg = wxString::Format(_("Delete series \"%s\"?"), current->GetValue(*model).c_str());
	bool ok = wxMessageBox(msg, _("Removing"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		FbCommonDatabase database;
		FbAutoCommit commit(database);
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM sequences WHERE id=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM bookseq WHERE id_seq=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM fts_seqn WHERE id=%d"), id));
		m_MasterList->Delete();
	}
}

FbFrameSequen::MenuBar::MenuBar()
{
	Append(new MenuFile,   _("&File"));
	Append(new MenuLib,    _("&Library"));
	Append(new MenuFrame,  _("&Catalog"));
	Append(new MenuMaster, _("&Series"));
	Append(new MenuBook,   _("&Books"));
	Append(new MenuView,   _("&View"));
	Append(new MenuSetup,  _("&Tools"));
	Append(new MenuWindow, _("&Window"));
	Append(new MenuHelp,   _("&?"));
}

FbFrameSequen::MenuMaster::MenuMaster()
{
	Append(ID_MASTER_APPEND,  _("Append"));
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(ID_MASTER_DELETE,  _("Delete"));
}

wxMenuBar * FbFrameSequen::CreateMenuBar()
{
	return new MenuBar;
}

void FbFrameSequen::OnModel( FbArrayEvent& event )
{
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameSequen::OnArray( FbArrayEvent& event )
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}
