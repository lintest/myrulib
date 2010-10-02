#include "FbBookMenu.h"
#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbMasterInfo.h"
#include "FbMasterTypes.h"

FbMenuMap FbBookMenu::sm_map;

int FbBookMenu::sm_next;

int FbBookMenu::GetKey(int id)
{
	return sm_map[id];
}

int FbBookMenu::SetKey(int key)
{
	sm_map[++sm_next] = key;
	return sm_next;
}

void FbBookMenu::Init(const FbMasterInfo &master, bool bShowOrder)
{
	sm_map.empty();
	sm_next = ID_FAVORITES_ADD;

	Append(ID_OPEN_BOOK, _("Open book") + (wxString)wxT("\tEnter"));

	FbMasterDownInfo * down = wxDynamicCast(&master, FbMasterDownInfo);
	if (down) {
		Append(ID_DELETE_DOWNLOAD, _("Delete download query"));
		if (down->GetId()) Append(ID_DOWNLOAD_BOOK, _("Retry rownload"));
	} else {
		Append(ID_DOWNLOAD_BOOK, _("Download a file"));
	}

	if ( m_book > 0 ) Append(ID_SYSTEM_DOWNLOAD, _("Download via browser"));
	if ( m_book > 0 ) Append(ID_BOOK_PAGE, _("Online books page"));
	AppendSeparator();

	Append(wxID_SELECTALL, _("Select all") + (wxString)wxT("\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Undo selection"));
	Append(ID_SHOW_COLUMNS, _("Table columns"));
	AppendSeparator();

	Append(wxID_DELETE, _("Delete book"));
	AppendSeparator();

	if (bShowOrder) Append(wxID_ANY, _("Sort by"), new FbMenuSort);
	Append(ID_FILTER_SET, _("Filter setup"));
	AppendCheckItem(ID_FILTER_USE, _("Use filter"));
	AppendSeparator();

	AppendAuthorsMenu();
	AppendSeriesMenu();
	AppendSeparator();

	FbMasterFldrInfo * info = wxDynamicCast(&master, FbMasterFldrInfo);
	int folder = info ? info->GetId() : 0;
	if (!info || folder) Append(ID_FAVORITES_ADD, _("Add to favourites"));
	AppendFoldersMenu(folder);
	Append(wxID_ANY, _("Rate this book"), new FbMenuRating);
	if (info) Append(ID_FAVORITES_DEL, _("Delete bookmark"));
	AppendSeparator();

	Append(ID_EDIT_COMMENTS, _("Add comments"));
}

void FbBookMenu::AppendAuthorsMenu()
{
	wxString text = _("Jump to author");
	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, full_name FROM authors WHERE id IN (SELECT id_author FROM books WHERE id=?) ORDER BY search_name");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_book);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	while (result.NextRow()) {
		wxString text = result.GetString(1);
		if (text.IsEmpty()) continue;
		if (submenu == NULL) submenu = new wxMenu;
		int id = SetKey(result.GetInt(0));
		submenu->Append(id, text);
		m_frame->Connect(id, wxEVT_COMMAND_MENU_SELECTED, m_auth_func);
	}
	Append(wxID_ANY, text, submenu)->Enable(submenu);
}

void FbBookMenu::AppendSeriesMenu()
{
	wxString text = _("Jump to series");
	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, value FROM sequences WHERE id IN (SELECT id_seq FROM bookseq WHERE id_book=?) ORDER BY value");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, m_book);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	while (result.NextRow()) {
		wxString text = result.GetString(1);
		if (text.IsEmpty()) continue;
		if (submenu == NULL) submenu = new wxMenu;
		int id = SetKey(result.GetInt(0));
		submenu->Append(id, text);
		m_frame->Connect(id, wxEVT_COMMAND_MENU_SELECTED, m_seqn_func);
	}
	Append(wxID_ANY, text, submenu)->Enable(submenu);
}

void FbBookMenu::AppendFoldersMenu(int folder)
{
	wxString text = _("Add to folders");
	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);

	while (result.NextRow()) {
		int key = result.GetInt(0);
		if (folder == key) continue;
		if (submenu == NULL) submenu = new wxMenu;
		int id = SetKey(result.GetInt(0));
		submenu->Append(id, result.GetString(1));
		m_frame->Connect(id, wxEVT_COMMAND_MENU_SELECTED, m_fldr_func);
	}
	Append(wxID_ANY, text, submenu)->Enable(submenu);
}
