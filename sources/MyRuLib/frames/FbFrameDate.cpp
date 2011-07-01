#include "FbFrameDate.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "dialogs/FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbMasterTypes.h"
#include "models/FbDateTree.h"
#include "FbWindow.h"
#include "FbParams.h"

IMPLEMENT_CLASS(FbFrameDate, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameDate, FbFrameBase)
	EVT_FB_MODEL(ID_MODEL_CREATE, FbFrameDate::OnModel)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameDate::OnBooksCount)
	EVT_COMMAND(ID_MODEL_NUMBER, fbEVT_BOOK_ACTION, FbFrameDate::OnNumber)
END_EVENT_TABLE()

FbFrameDate::FbFrameDate(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_DATE, GetTitle(), select)
{
	m_MasterList = new FbMasterViewCtrl;
	m_MasterList->Create(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	CreateColumns();

	CreateBooksPanel(this);
	SplitVertically(m_MasterList, m_BooksPanel);

	CreateControls(select);

	m_MasterThread = new FbDateTreeThread(this, m_MasterFile);
	m_MasterThread->Execute();
}

void FbFrameDate::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Date"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

void FbFrameDate::OnBooksCount(FbCountEvent& event)
{
	FbDateDayData * child = wxDynamicCast(&m_MasterList->GetCurrent(), FbDateDayData);
	if (child && *child == event.GetInfo()) {
		child->SetCount(event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

void FbFrameDate::OnModel( FbModelEvent& event )
{
	m_MasterList->AssignModel(event.GetModel());
}

void FbFrameDate::OnNumber(wxCommandEvent& event)
{
	m_MasterFile = event.GetString();
	FbDateTreeModel * model = wxDynamicCast(m_MasterList->GetModel(), FbDateTreeModel);
	if (model) model->SetCounter(m_MasterFile);
	m_MasterList->Refresh();
}
