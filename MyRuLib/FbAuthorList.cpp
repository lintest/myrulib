#include "FbAuthorList.h"
#include "FbClientData.h"
#include "ImpContext.h"
#include "FbManager.h"
#include "FbConst.h"
#include "FbAuthorDlg.h"
#include "FbBookEvent.h"
#include "FbReplaceDlg.h"

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
	AddColumn(_("Автор"), 40, wxALIGN_LEFT);
	AddColumn(_("Кол."), 10, wxALIGN_RIGHT);
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
	int id = FbAuthorDlg::Append();
	if (id) FbOpenEvent(ID_BOOK_AUTHOR, id).Post();
}

void FbAuthorList::OnAuthorModify(wxCommandEvent& event)
{
	FbAuthorData * data = (FbAuthorData*) GetSelected();
	if (!data) return;

	int id = FbAuthorDlg::Modify(data->GetId());
	if (id) FbOpenEvent(ID_BOOK_AUTHOR, id).Post();
}

void FbAuthorList::OnAuthorDelete(wxCommandEvent& event)
{
	wxTreeItemId selected = GetSelection();
	FbAuthorData * data = (FbAuthorData*) GetSelected();
	if (!data) return;
	int id = data->GetId();

	wxString sql = wxT("SELECT count(id) FROM books WHERE id_author=?");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	int count = result.NextRow() ? result.GetInt(0) : 0;

	wxString msg = _("Удалить автора: ") + GetItemText(selected);
	if (count) msg += wxString::Format(_("\nи все его книги в количестве %d шт.?"), count);
	bool ok = wxMessageBox(msg, _("Удаление"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK;
	if (ok) {
		(new DeleteThread(id))->Execute();
		Delete(selected);
	}
}


void FbAuthorList::OnAuthorReplace(wxCommandEvent& event)
{
	wxTreeItemId selected = GetSelection();
	FbAuthorData * data = (FbAuthorData*) GetSelected();
	if (!data) return;

	int id = FbReplaceDlg::Execute(data->GetId());
	if (id) FbOpenEvent(ID_BOOK_AUTHOR, id).Post();
}

void * FbAuthorList::DeleteThread::Entry()
{
	wxCriticalSectionLocker locker(sm_queue);

	FbCommonDatabase database;
	wxString sql;

	sql = wxString::Format(wxT("books.id_author=%d"), m_author);
	LogDelete(database, sql);

	sql = wxString::Format(wxT("DELETE FROM bookseq WHERE id_book IN (SELECT id FROM books WHERE id_author=%d)"), m_author);
	ExecSQL(database, sql);

	sql = wxString::Format(wxT("DELETE FROM files WHERE id_book IN (SELECT id FROM books WHERE id_author=%d)"), m_author);
	ExecSQL(database, sql);

	sql = wxString::Format(wxT("DELETE FROM books WHERE id_author=%d"), m_author);
	ExecSQL(database, sql);

	sql = wxString::Format(wxT("DELETE FROM authors WHERE id=%d"), m_author);
	ExecSQL(database, sql);

	return NULL;
}
