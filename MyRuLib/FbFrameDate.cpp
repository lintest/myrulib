#include "FbFrameDate.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "InfoCash.h"
#include "FbClientData.h"
#include "ExternalDlg.h"
#include "FbMainMenu.h"
#include "FbUpdateThread.h"
#include "FbDateTree.h"
#include "FbWindow.h"
#include "FbParams.h"

BEGIN_EVENT_TABLE(FbFrameDate, FbFrameBase)
	EVT_TREE_SEL_CHANGED(ID_MASTER_LIST, FbFrameDate::OnMasterSelected)
	EVT_COMMAND(ID_BOOKS_COUNT, fbEVT_BOOK_ACTION, FbFrameDate::OnBooksCount)
	EVT_FB_ARRAY(ID_MODEL_CREATE, FbFrameDate::OnModel)
	EVT_FB_ARRAY(ID_MODEL_APPEND, FbFrameDate::OnArray)
END_EVENT_TABLE()

FbFrameDate::FbFrameDate(wxAuiMDIParentFrame * parent)
	:FbFrameBase(parent, ID_FRAME_DATE, GetTitle()), m_FindText(NULL), m_FindInfo(NULL), m_SequenceCode(0)
{
	CreateControls();
}

void FbFrameDate::CreateControls()
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
	CreateColumns();

	long substyle = wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxTR_MULTIPLE | wxSUNKEN_BORDER;
	CreateBooksPanel(splitter, substyle);
	splitter->SplitVertically(m_MasterList, m_BooksPanel, 160);

	FbFrameBase::CreateControls();

	m_MasterThread = new FbDateTreeThread(this);
	m_MasterThread->Execute();
}

void FbFrameDate::Localize(bool bUpdateMenu)
{
	m_MasterList->EmptyColumns();
	CreateColumns();
	FbFrameBase::Localize(bUpdateMenu);
}

void FbFrameDate::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Date"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

void FbFrameDate::OnMasterSelected(wxTreeEvent & event)
{
	UpdateBooklist();
}

void FbFrameDate::UpdateBooklist()
{
	m_BooksPanel->EmptyBooks();
	FbDateDayData * data = wxDynamicCast(m_MasterList->GetCurrent(), FbDateDayData);
	if (data) FbMasterDate(data->GetCode()).Show(this);
	else if (m_BooksPanel) m_BooksPanel->AssignEmpty();
}

void FbFrameDate::OnBooksCount(wxCommandEvent& event)
{
/*
	wxTreeItemId item = m_MasterList->GetSelection();
	if (item.IsOk()) m_MasterList->SetItemText(item, 1, wxString::Format(wxT("%d"), GetBookCount()));
	event.Skip();
*/
}

void FbFrameDate::OnModel( FbArrayEvent& event )
{
	FbTreeModel * model = new FbTreeModel();
	model->SetRoot(new FbParentData(*model));
	AppendAttay(*model, event.GetArray());
	m_MasterList->AssignModel(model);
}

void FbFrameDate::OnArray( FbArrayEvent& event )
{
	FbTreeModel * model = wxDynamicCast(m_MasterList->GetModel(), FbTreeModel);
	if (model) AppendAttay(*model, event.GetArray());
	m_MasterList->Refresh();
}

void FbFrameDate::AppendAttay(FbTreeModel &model, const wxArrayInt &items)
{
	FbParentData * root = wxDynamicCast(model.GetRoot(), FbParentData);
	if (root == NULL) return;

	size_t count = items.Count();
	if (count == 0) return;

	int month = 0;
	FbParentData * parent_year = new FbDateYearData(model, root, items[0] / 10000);
	FbParentData * parent_month = NULL;
	for (size_t i = 0; i < count; ) {
		int day = items[i++];
		int count = items[i++];
		int new_month = day / 100;
		if (month != new_month) {
			parent_month = new FbDateMonthData(model, parent_year, new_month);
			month = new_month;
		}
		new FbDateDayData(model, parent_month, day, count);
	}
}
