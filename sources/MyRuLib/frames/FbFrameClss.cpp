#include "FbFrameClss.h"
#include <wx/artprov.h>
#include <wx/splitter.h>
#include "FbConst.h"
#include "FbClientData.h"
#include "dialogs/FbExportDlg.h"
#include "FbMainMenu.h"
#include "FbMasterTypes.h"
#include "models/FbClssTree.h"
#include "FbWindow.h"
#include "FbParams.h"

IMPLEMENT_CLASS(FbFrameClss, FbFrameBase)

BEGIN_EVENT_TABLE(FbFrameClss, FbFrameBase)
	EVT_TREE_ITEM_ACTIVATED(ID_MASTER_LIST, FbFrameClss::OnItemActivated)
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbFrameClss::OnContextMenu)
	EVT_FB_COUNT(ID_BOOKS_COUNT, FbFrameClss::OnBooksCount)
END_EVENT_TABLE()

FbFrameClss * FbFrameClss::Create(wxAuiNotebook * parent, int code, bool select)
{
	FbCommonDatabase database;
	if (!database.TableExists(wxT("tables"))) return NULL;
	wxString sql = wxT("SELECT * FROM tables WHERE id="); sql << code;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	return result.NextRow() ? new FbFrameClss(parent, result, select) : NULL;
}

FbFrameClss::FbFrameClss(wxAuiNotebook * parent, wxSQLite3ResultSet & result, bool select)
	: FbFrameBase(parent, ID_FRAME_CLSS, result.GetString(wxT("title")), select)
	, m_code(result.GetInt(wxT("id")))
{
	m_MasterList = new FbMasterViewCtrl;
	m_MasterList->Create(this, ID_MASTER_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES|fbTR_DIRECTORY);
	CreateColumns();

	CreateBooksPanel(this);
	SplitVertically(m_MasterList, m_BooksPanel);

	CreateControls(select);

	CreateModel(result);
}

void FbFrameClss::CreateModel(wxSQLite3ResultSet & result)
{
	FbTreeModel * model = new FbClssTreeModel(result);
	FbParentData * root = new FbClssModelData(*model, result.GetString(wxT("title")));
	root->Expand(*model, true);
	model->SetRoot(root);
	model->GoNextRow();
	m_MasterList->AssignModel(model);
}

void FbFrameClss::CreateColumns()
{
	m_MasterList->AddColumn(0, _("Classifier"), 40, wxALIGN_LEFT);
	m_MasterList->AddColumn(1, _("Num."), 10, wxALIGN_RIGHT);
}

void FbFrameClss::OnBooksCount(FbCountEvent& event)
{
	FbClssModelData * child = wxDynamicCast(&m_MasterList->GetCurrent(), FbClssModelData);
	if (child && *child == event.GetInfo()) {
		child->SetCount(event.GetCount());
		m_MasterList->Refresh();
	}
	event.Skip();
}

void FbFrameClss::OnItemActivated(wxTreeEvent & event)
{
	FbModelItem item = m_MasterList->GetCurrent();
	item.Expand(!item.IsExpanded());
}

void FbFrameClss::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	if (point.x == -1 && point.y == -1) {
		wxSize size = m_MasterList->GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbFrameClss::ShowContextMenu(const wxPoint& pos, wxTreeItemId)
{
	FbModelItem item = m_MasterList->GetCurrent();
	FbClssModelData * data = wxDynamicCast(&item, FbClssModelData);
	wxString code = data ? data->GetCode() : wxString();
	MasterMenu menu(code);
	m_MasterList->PopupMenu(&menu, pos.x, pos.y);
}

FbFrameClss::MasterMenu::MasterMenu(const wxString & code)
{
	Append(ID_MASTER_APPEND,  _("Append"));
	if (code.IsEmpty() || code == wxT('0')) return;
	Append(ID_MASTER_MODIFY,  _("Modify"));
	Append(wxID_DELETE,       _("Delete"));
}

