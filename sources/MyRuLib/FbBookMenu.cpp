#include "FbBookMenu.h"
#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbMasterInfo.h"
#include "FbMasterTypes.h"
#include "FbBookList.h"
#include "FbBookTree.h"

FbMenuMap FbBookMenu::sm_key;

FbMenuMap FbBookMenu::sm_type;

int FbBookMenu::sm_next;

FbBookMenu::FbBookMenu(wxWindow * frame, FbModelItem item, int book)
	: m_frame(frame), m_auth(0), m_seqn(0), m_book(book)
{
	if (book) return;

	FbAuthParentData * auth = wxDynamicCast(&item, FbAuthParentData);
	if (auth) {
		m_auth = auth->GetCode();
		FbSeqnParentData * seqn = wxDynamicCast(&item.GetParent(), FbSeqnParentData);
		if (seqn) m_seqn = seqn->GetCode();
		return;
	}

	FbSeqnParentData * seqn = wxDynamicCast(&item, FbSeqnParentData);
	if (seqn) {
		m_seqn = seqn->GetCode();
		FbAuthParentData * auth = wxDynamicCast(&item.GetParent(), FbAuthParentData);
		if (auth) m_auth = auth->GetCode();
		return;
	}
}

int FbBookMenu::SetKey(int key, FbMenuType type)
{
	sm_next++;
	sm_key[sm_next] = key;
	sm_type[sm_next] = type;
	return sm_next;
}

bool FbBookMenu::GetKey(int id, int &key, FbMenuType &type)
{
	bool ok = sm_key.count(id) && sm_type.count(id);
	if (ok) {
		key = sm_key[id];
		type = (FbMenuType) sm_type[id];
	}
	return ok;
}

void FbBookMenu::Init(const FbMasterInfo &master, bool bShowOrder)
{
	sm_key.empty();
	sm_type.empty();
	sm_next = ID_FAVORITES_ADD;

	Append(ID_OPEN_BOOK, _("Open book") + (wxString)wxT("\tEnter"));

	FbMasterDownInfo * down = wxDynamicCast(&master, FbMasterDownInfo);
	if (down) {
		Append(ID_DELETE_DOWNLOAD, _("Delete download query"));
		if (down->GetId()) Append(ID_DOWNLOAD_BOOK, _("Retry download"));
	} else {
		Append(ID_DOWNLOAD_BOOK, _("Download a file"));
	}

	if ( m_book > 0 ) Append(ID_SYSTEM_DOWNLOAD, _("Download via browser"));
	if ( m_book > 0 ) Append(ID_BOOK_PAGE, _("Online books page"));
	AppendSeparator();

	Append(wxID_COPY, _("Copy") + (wxString)wxT("\tCtrl+C"));
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
	Append(wxID_PROPERTIES, _("Properties"));
}

void FbBookMenu::AppendAuthorsMenu()
{
	wxString text = _("Jump to author");
	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, full_name FROM authors WHERE %s ORDER BY search_name");
	if (m_book) {
		sql = wxString::Format(sql, wxT("id IN (SELECT id_author FROM books WHERE id=%d)"));
		sql = wxString::Format(sql, m_book);
	} else if (m_auth) {
		sql = wxString::Format(sql, wxT("id=%d"));
		sql = wxString::Format(sql, m_auth);
	} else {
		Append(wxID_ANY, text)->Enable(false);
		return;
	}

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		wxString text = result.GetString(1);
		if (text.IsEmpty()) continue;
		if (submenu == NULL) submenu = new wxMenu;
		int id = SetKey(result.GetInt(0), MenuAuth);
		submenu->Append(id, text);
	}
	Append(wxID_ANY, text, submenu)->Enable(submenu);
}

void FbBookMenu::AppendSeriesMenu()
{
	wxString text = _("Jump to series");
	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, value FROM sequences WHERE %s ORDER BY value");
	if (m_book) {
		sql = wxString::Format(sql, wxT("id IN (SELECT id_seq FROM bookseq WHERE id_book=%d)"));
		sql = wxString::Format(sql, m_book);
	} else if (m_seqn) {
		sql = wxString::Format(sql, wxT("id=%d"));
		sql = wxString::Format(sql, m_seqn);
	} else {
		Append(wxID_ANY, text)->Enable(false);
		return;
	}

	FbCommonDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	while (result.NextRow()) {
		wxString text = result.GetString(1);
		if (text.IsEmpty()) continue;
		if (submenu == NULL) submenu = new wxMenu;
		int id = SetKey(result.GetInt(0), MenuSeqn);
		submenu->Append(id, text);
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
		int id = SetKey(result.GetInt(0), MenuFldr);
		submenu->Append(id, result.GetString(1));
	}
	Append(wxID_ANY, text, submenu)->Enable(submenu);
}


