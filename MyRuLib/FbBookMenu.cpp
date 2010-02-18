#include "FbBookMenu.h"
#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbMasterData.h"

WX_DEFINE_OBJARRAY(FbMenuFolderArray);

WX_DEFINE_OBJARRAY(FbMenuAuthorArray);

FbBookMenu::FbBookMenu(int id, const FbMasterData &data, bool bShowOrder)
	: m_id(id)
{
	Append(ID_OPEN_BOOK, _("Open book") + (wxString)wxT("\tEnter"));
	if (data.GetType() == FT_DOWNLOAD) {
		Append(ID_DELETE_DOWNLOAD, _("Delete download query"));
		if ( data.GetId() < 0) Append(ID_DOWNLOAD_BOOK, _("Retry rownload"));
	} else {
		Append(ID_DOWNLOAD_BOOK, _("Download a file"));
	}
	if ( id>0 ) Append(ID_SYSTEM_DOWNLOAD, _("Download via browser"));
	if ( id>0 ) Append(ID_BOOK_PAGE, _("Online books page"));
	AppendSeparator();

	Append(wxID_SELECTALL, _("Select all") + (wxString)wxT("\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Undo selection"));
	Append(ID_SHOW_COLUMNS, _("Table columns"));
	AppendSeparator();

//	Append(wxID_EDIT, _("Редактировать"));
	Append(wxID_DELETE, _("Delete book"));
	AppendSeparator();

	if (bShowOrder) Append(wxID_ANY, _("Sort by"), new FbMenuSort);
	AppendCheckItem(ID_FILTER_USE, _("Use filter"));
	Append(ID_FILTER_NOT, _("Clear filter"));
	AppendSeparator();

	Append(wxID_ANY, _("Jump to author"), new FbMenuAuthors(m_id));
//	Append(wxID_ANY, _("Открыть серию"), NULL);
	AppendSeparator();

	if (data.GetType() != FT_FOLDER || data.GetId()) Append(ID_FAVORITES_ADD, _("Add to favourites"));
	Append(wxID_ANY, _("Add to folders"), new FbMenuFolders(data));
	Append(wxID_ANY, _("Rate this book"), new FbMenuRating);
	if (data.GetType() == FT_FOLDER) Append(ID_FAVORITES_DEL, _("Delete bookmark"));
	AppendSeparator();

	Append(ID_EDIT_COMMENTS, _("Comments"));
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

FbMenuFolders::FbMenuFolders(const FbMasterData &data)
{
	if (sm_folders.IsEmpty()) LoadFolders();

	for (size_t i=0; i<sm_folders.Count(); i++) {
		int id = sm_folders[i].id;
		if (data.GetType() == FT_FOLDER && sm_folders[i].folder == data.GetId()) continue;
		Append(id, sm_folders[i].name);
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
	if (sm_folders.IsEmpty()) LoadFolders();
	for (size_t i=0; i<sm_folders.Count(); i++)
		frame->Connect(sm_folders[i].id, wxEVT_COMMAND_MENU_SELECTED, func);
}

size_t FbMenuFolders::GetCount()
{
	if (sm_folders.IsEmpty()) LoadFolders();
	return sm_folders.Count();
};
