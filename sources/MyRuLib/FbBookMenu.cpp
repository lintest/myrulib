#include "FbBookMenu.h"
#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbMasterInfo.h"
#include "FbMasterTypes.h"

WX_DEFINE_OBJARRAY(FbMenuFolderArray);

WX_DEFINE_OBJARRAY(FbMenuAuthorArray);

FbBookMenu::FbBookMenu(const FbMasterInfo &master, int book, bool bShowOrder)
	: m_id(book)
{
	Append(ID_OPEN_BOOK, _("Open book") + (wxString)wxT("\tEnter"));

	FbMasterDownInfo * down = wxDynamicCast(&master, FbMasterDownInfo);
	if (down) {
		Append(ID_DELETE_DOWNLOAD, _("Delete download query"));
		if ( down->GetId() < 0) Append(ID_DOWNLOAD_BOOK, _("Retry rownload"));
	} else {
		Append(ID_DOWNLOAD_BOOK, _("Download a file"));
	}

	if ( book > 0 ) Append(ID_SYSTEM_DOWNLOAD, _("Download via browser"));
	if ( book > 0 ) Append(ID_BOOK_PAGE, _("Online books page"));
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

	Append(wxID_ANY, _("Jump to author"), new FbMenuAuthors(book));
	Append(wxID_ANY, _("Jump to series"), NULL);
	AppendSeparator();

	FbMasterFldrInfo * fldr = wxDynamicCast(&master, FbMasterFldrInfo);
	int folder = fldr ? fldr->GetId() : 0;
	if (!fldr || folder) Append(ID_FAVORITES_ADD, _("Add to favourites"));
	Append(wxID_ANY, _("Add to folders"), new FbMenuFolders(folder));
	Append(wxID_ANY, _("Rate this book"), new FbMenuRating);
	if (fldr) Append(ID_FAVORITES_DEL, _("Delete bookmark"));
	AppendSeparator();

	Append(ID_EDIT_COMMENTS, _("Add comments"));
}

FbMenuAuthorArray FbMenuAuthors::sm_authors;

FbMenuAuthors::FbMenuAuthors(int book)
{
	sm_authors.Empty();

	wxString sql = wxT("SELECT id, full_name FROM authors WHERE id IN (SELECT id_author FROM books WHERE id=?) ORDER BY search_name");
	FbCommonDatabase database;
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, book);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();

	int id = ID_FAVORITES_ADD + FbMenuFolders::GetCount();
	while (result.NextRow()) {
		FbMenuAuthorItem * item = new FbMenuAuthorItem;
		item->id = ++id;
		item->author = result.GetInt(0);
		Append(item->id, result.GetString(1));
		sm_authors.Add(item);
	}
}

int FbMenuAuthors::GetAuthor(const int id)
{
	for (size_t i=0; i<sm_authors.Count(); i++)
		if ( sm_authors[i].id == id ) return sm_authors[i].author;
	return 0;
}

void FbMenuAuthors::Connect(wxWindow * frame, wxObjectEventFunction func)
{
	for (size_t i=0; i<sm_authors.Count(); i++)
		frame->Connect(sm_authors[i].id, wxEVT_COMMAND_MENU_SELECTED, func);
}

FbMenuFolderArray FbMenuFolders::sm_folders;

FbMenuFolders::FbMenuFolders(int folder)
{
	LoadFolders();
	for (size_t i=0; i<sm_folders.Count(); i++) {
		if (folder == sm_folders[i].folder) continue;
		Append(sm_folders[i].id, sm_folders[i].name);
	}
}

void FbMenuFolders::LoadFolders()
{
	sm_folders.Empty();
	wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
	FbLocalDatabase database;
	wxSQLite3ResultSet result = database.ExecuteQuery(sql);
	int id = ID_FAVORITES_ADD;
	while (result.NextRow()) {
		FbMenuFolderItem * item = new FbMenuFolderItem;
		item->id = ++id;
		item->folder = result.GetInt(0);
		item->name = result.GetString(1);
		sm_folders.Add(item);
	}
}

int FbMenuFolders::GetFolder(const int id)
{
	for (size_t i=0; i<sm_folders.Count(); i++)
		if ( sm_folders[i].id == id ) return sm_folders[i].folder;
	return 0;
}

void FbMenuFolders::Connect(wxWindow * frame, wxObjectEventFunction func)
{
	for (size_t i=0; i<sm_folders.Count(); i++)
		frame->Connect(sm_folders[i].id, wxEVT_COMMAND_MENU_SELECTED, func);
}

size_t FbMenuFolders::GetCount()
{
	if (sm_folders.IsEmpty()) LoadFolders();
	return sm_folders.Count();
};
