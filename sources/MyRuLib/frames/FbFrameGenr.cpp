#include "FbFrameGenr.h"
#include <wx/artprov.h>
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbGenres.h"
#include "FbBookPanel.h"
#include "models/FbGenrTree.h"
#include "FbMainMenu.h"

IMPLEMENT_CLASS(FbFrameGenr, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameGenr, FbFrameBase)
	EVT_FB_MODEL(ID_MODEL_CREATE, FbFrameGenr::OnModel)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameGenr::OnBooksCount)
END_EVENT_TABLE()

FbFrameGenr::FbFrameGenr(wxAuiNotebook * parent, bool select)
	: FbFrameBase(parent, ID_FRAME_GENR, GetTitle(), select)
{
	m_MasterList = new FbMasterViewCtrl;
	m_MasterList->Create(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, FbParams.Style());

	CreateBooksPanel(this);
	SplitVertically(m_MasterList, m_BooksPanel);

	CreateControls(select);
	CreateColumns();

	UpdateCounter();
}

void FbFrameGenr::CreateColumns()
{
	m_MasterList->AddColumn (0, _("List of genres"), -10, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 6, wxALIGN_RIGHT);
	FbModel * model = FbGenres::CreateModel(wxT("11"));
	m_MasterList->AssignModel(model);
}

wxString FbFrameGenr::GetCountSQL()
{
    return wxT("SELECT IFNULL(id_genre,''),COUNT(DISTINCT id) FROM books LEFT JOIN genres ON books.id=genres.id_book WHERE 1 %s GROUP BY IFNULL(id_genre,'')");
}

FbFrameThread * FbFrameGenr::CreateCounter()
{
	FbFrameThread * thread = new FbGenrListThread(this);
	thread->SetCountSQL(GetCountSQL(), m_filter);
	return thread;
}

void FbFrameGenr::OnModel( FbModelEvent & event )
{
	FbSmartPtr<FbListStore> list = wxDynamicCast(event.GetModel(), FbListStore);
	FbTreeModel * tree = wxDynamicCast(m_MasterList->GetModel(), FbTreeModel);
	if (!tree || !list) return;

	FbModelItem root = tree->GetRoot();
	if (!root) return;

	size_t count = root.Count();
	for (size_t j = 0; j < count; j++) {
		FbModelItem parent = root.Items(j);
		size_t count = parent.Count();
		for (size_t k = 0; k < count; k++) {
			FbGenrChildData * child = wxDynamicCast(&parent.Items(k), FbGenrChildData);
			if (!child) continue;
			child->SetCount(0);
			size_t count = list->GetRowCount();
			for (size_t i = 1; i <= count; i++) {
				FbModelItem item = list->GetData(i);
				FbGenrListData * data = wxDynamicCast(&item, FbGenrListData);
				if (data && *child == *data) { *child = *data; break; }
			}
		}
	}
	m_MasterList->Refresh();
}

void FbFrameGenr::OnBooksCount(FbCountEvent& event)
{
	FbGenrChildData * child = wxDynamicCast(&m_MasterList->GetCurrent(), FbGenrChildData);
	if (child && *child == event.GetInfo()) {
		child->SetCount(event.GetCount());
		m_MasterList->Refresh();
	}

	event.Skip();
}

