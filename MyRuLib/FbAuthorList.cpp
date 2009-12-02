#include "FbAuthorList.h"
#include "FbClientData.h"
#include "ImpContext.h"
#include "FbManager.h"
#include "FbConst.h"
#include "FbAuthorDlg.h"

class FbAuthorMenu: public wxMenu
{
	public:
		FbAuthorMenu(int id);
};

FbAuthorMenu::FbAuthorMenu(int id)
{
	Append(ID_AUTHOR_APPEND, _("Добавить"));
	if (id == 0) return;
	Append(ID_AUTHOR_MODIFY, _("Изменить"));
	Append(ID_AUTHOR_DELETE, _("Удалить"));
	AppendSeparator();
	Append(ID_AUTHOR_REPLACE, _("Заменить"));
}

BEGIN_EVENT_TABLE(FbAuthorList, FbTreeListCtrl)
	EVT_TREE_ITEM_MENU(ID_MASTER_LIST, FbAuthorList::OnContextMenu)
	EVT_MENU(ID_AUTHOR_APPEND, FbAuthorList::OnAuthorAppend)
	EVT_MENU(ID_AUTHOR_MODIFY, FbAuthorList::OnAuthorModify)
	EVT_MENU(ID_AUTHOR_DELETE, FbAuthorList::OnAuthorDelete)
	EVT_MENU(ID_AUTHOR_REPLACE, FbAuthorList::OnAuthorReplace)
END_EVENT_TABLE()

FbAuthorList::FbAuthorList(wxWindow * parent, wxWindowID id)
	:FbTreeListCtrl(parent, id, wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_COLUMN_LINES | wxSUNKEN_BORDER)
{
	this->AddColumn(_("Автор"), 40, wxALIGN_LEFT);
	this->AddColumn(_("Кол."), 10, wxALIGN_RIGHT);
}

void FbAuthorList::OnContextMenu(wxTreeEvent& event)
{
	wxPoint point = event.GetPoint();
	// If from keyboard
	if (point.x == -1 && point.y == -1) {
		wxSize size = GetSize();
		point.x = size.x / 3;
		point.y = size.y / 3;
	}
	ShowContextMenu(point, event.GetItem());
}

void FbAuthorList::ShowContextMenu(const wxPoint& pos, wxTreeItemId item)
{
	int id = 0;
	if (item.IsOk()) {
		FbAuthorData * data = (FbAuthorData*)GetItemData(item);
		if (data) id = data->GetId();
	}
	FbAuthorMenu menu(id);
	PopupMenu(&menu, pos.x, pos.y);
}

FbAuthorData * FbAuthorList::GetSelected()
{
	wxTreeItemId selected = GetSelection();
	if (selected.IsOk()) {
		return (FbAuthorData*) GetItemData(selected);
	} else
		return NULL;
}

void FbAuthorList::OnAuthorAppend(wxCommandEvent& event)
{
	FbAuthorDlg::Append();
}

void FbAuthorList::OnAuthorModify(wxCommandEvent& event)
{
	FbAuthorData * data = (FbAuthorData*) GetSelected();
	if (!data) return;
	FbAuthorDlg::Modify(data->GetId());
}

void FbAuthorList::OnAuthorDelete(wxCommandEvent& event)
{
}

void FbAuthorList::OnAuthorReplace(wxCommandEvent& event)
{
}
