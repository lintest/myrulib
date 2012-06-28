#include "FbFrameSeqn.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbBookPanel.h"
#include "FbClientData.h"
#include "dialogs/FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbWindow.h"
#include "FbParams.h"
#include "dialogs/FbSequenDlg.h"
#include "FbMasterTypes.h"

//-----------------------------------------------------------------------------
//  FbSeqnViewCtrl
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FbSeqnViewCtrl, FbMasterViewCtrl)
	EVT_MENU(wxID_DELETE, FbSeqnViewCtrl::OnMasterDelete)
	EVT_MENU(ID_MASTER_APPEND, FbSeqnViewCtrl::OnMasterAppend)
	EVT_MENU(ID_MASTER_MODIFY, FbSeqnViewCtrl::OnMasterModify)
	EVT_UPDATE_UI(ID_MASTER_APPEND, FbMasterViewCtrl::OnEnableUI)
	EVT_UPDATE_UI(ID_MASTER_MODIFY, FbMasterViewCtrl::OnEnableUI)
END_EVENT_TABLE()

void FbSeqnViewCtrl::OnMasterAppend(wxCommandEvent& event)
{
	wxString newname;
	int id = FbSequenDlg::Append(newname);
	if (id == 0) return;

	Append(new FbSeqnListData(id));
}

void FbSeqnViewCtrl::OnMasterModify(wxCommandEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(GetModel(), FbSeqnListModel);
	if (model == NULL) return;

	FbModelItem item = GetCurrent();
	FbSeqnListData * current = wxDynamicCast(&item, FbSeqnListData);
	if (current == NULL) return;

	wxString newname;
	int old_id = current->GetCode();
	int new_id = FbSequenDlg::Modify(old_id, newname);
	if (new_id == 0) return;

	if (new_id != old_id) model->Delete(new_id);
	Replace(new FbSeqnListData(new_id));
}

void FbSeqnViewCtrl::OnMasterDelete(wxCommandEvent& event)
{
	FbModel * model = GetModel();
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
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM s WHERE sid=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM bs WHERE sid=%d"), id));
		database.ExecuteUpdate(wxString::Format(wxT("DELETE FROM fts_s WHERE docid=%d"), id));
		Delete();
	}
}

//-----------------------------------------------------------------------------
//  FbAuthViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbFrameSeqn, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameSeqn, FbFrameBase)
	EVT_LIST_COL_CLICK(ID_MASTER_LIST, FbFrameSeqn::OnColClick)
	EVT_TEXT_ENTER(ID_MASTER_FIND, FbFrameSeqn::OnFindEnter )
	EVT_BUTTON(ID_MASTER_FIND, FbFrameSeqn::OnFindEnter )
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameSeqn::OnContextMenu)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameSeqn::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameSeqn::OnArray)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameSeqn::OnBooksCount)
	EVT_COMMAND(ID_MODEL_NUMBER, fbEVT_BOOK_ACTION, FbFrameSeqn::OnNumber)
END_EVENT_TABLE()

FbFrameSeqn::FbFrameSeqn(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_SEQN, GetTitle(), select),
		m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	wxPanel * panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

	m_FindText = new FbSearchCombo( panel, ID_MASTER_FIND, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_FindText->SetMinSize( wxSize( 200,-1 ) );

	m_MasterList = new FbSeqnViewCtrl;
	m_MasterList->Create(panel, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, FbParams.Style());
	m_MasterList->SetSortedColumn(1);
	CreateColumns();

	CreateBooksPanel(this);

	sizer->Add( m_FindText, 0, wxEXPAND, 0 );
	sizer->Add( m_MasterList, 1, wxTOP|wxEXPAND, 2 );
	panel->SetSizer( sizer );
	panel->Layout();
	sizer->Fit( panel );

	SplitVertically(panel, m_BooksPanel);

	CreateControls(select);

	FindSequence(wxEmptyString);
}

void FbFrameSeqn::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Ser."), -10, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 6, wxALIGN_RIGHT);
}

wxString FbFrameSeqn::GetCountSQL()
{
	return wxT("SELECT id_seq, COUNT(DISTINCT id) FROM books INNER JOIN bookseq ON books.id=bookseq.id_book WHERE 1 %s GROUP BY id_seq");
}

void FbFrameSeqn::CreateMasterThread()
{
	m_MasterList->AssignModel(NULL);
	if (m_MasterThread) {
		m_MasterThread->Close();
		m_MasterThread->Wait();
		wxDELETE(m_MasterThread);
	}
	m_MasterThread = new FbSeqnListThread(this, m_info, m_MasterList->GetSortedColumn(), m_MasterFile);
	m_MasterThread->SetCountSQL(GetCountSQL(), m_filter);
	m_MasterThread->Execute();
}

void FbFrameSeqn::FindSequence(const wxString &text)
{
	m_info = text;
	CreateMasterThread();
}

void FbFrameSeqn::OpenSequence(const int sequence, const int book)
{
/*
	m_SequenceText = wxEmptyString;
	m_SequenceCode = sequence;
	(new MasterThread(m_MasterList, m_SequenceCode, m_MasterList->GetSortedColumn()))->Execute();
*/
}

void FbFrameSeqn::OnColClick(wxListEvent& event)
{
	CreateMasterThread();
}

void FbFrameSeqn::OnBooksCount(FbCountEvent& event)
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	FbMasterSeqnInfo * info = wxDynamicCast(&event.GetInfo(), FbMasterSeqnInfo);
	if (model && info) {
		model->SetCount(info->GetId(), event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

void FbFrameSeqn::OnFindEnter(wxCommandEvent& event)
{
	FindSequence(m_FindText->GetValue());
}

FbFrameSeqn::MasterMenu::MasterMenu(int id)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (id == 0) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(wxID_DELETE,       _("Delete"));
}

void FbFrameSeqn::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameSeqn::ShowContextMenu(const wxPoint& pos, wxTreeItemId)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbSeqnListData * data = wxDynamicCast(&item, FbSeqnListData);
	int id = data ? data->GetCode() : 0;
	MasterMenu menu(id);
	m_MasterList->PopupMenu(&menu, pos.x, pos.y);
}

void FbFrameSeqn::OnModel( FbArrayEvent& event )
{
	FbSeqnListModel * model = new FbSeqnListModel(event.GetArray());
	model->SetCounter(m_MasterFile);
	m_MasterList->AssignModel(model);
}

void FbFrameSeqn::OnArray( FbArrayEvent& event )
{
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) model->Append(event.GetArray());
	m_MasterList->Refresh();
}

void FbFrameSeqn::OnNumber(wxCommandEvent& event)
{
	m_MasterFile = event.GetString();
	FbSeqnListModel * model = wxDynamicCast(m_MasterList->GetModel(), FbSeqnListModel);
	if (model) model->SetCounter(m_MasterFile);
	m_MasterList->Refresh();
}
