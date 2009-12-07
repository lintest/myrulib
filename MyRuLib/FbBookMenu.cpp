#include "FbBookMenu.h"
#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"

WX_DEFINE_OBJARRAY(FbMenuFolderArray);

WX_DEFINE_OBJARRAY(FbMenuAuthorArray);

FbBookMenu::FbBookMenu(int id, int iFolder, int iType, bool bShowOrder)
	: m_id(id)
{
	Append(ID_OPEN_BOOK, _("Открыть книгу\tEnter"));
	if (iType == FT_DOWNLOAD) {
		Append(ID_DELETE_DOWNLOAD, _("Удалить закачку"));
		if (iFolder < 0) Append(ID_DOWNLOAD_BOOK, _("Скачать повторно"));
	} else {
		Append(ID_DOWNLOAD_BOOK, _("Скачать файл"));
	}
	if ( id>0 ) Append(ID_SYSTEM_DOWNLOAD, _("Скачать в браузере"));
	AppendSeparator();

	Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Отменить выделение"));
	AppendSeparator();

//	Append(wxID_EDIT, _("Редактировать"));
	Append(wxID_DELETE, _("Удалить книгу"));
	AppendSeparator();

	if (bShowOrder) Append(wxID_ANY, _("Сортировка"), new FbMenuSort);
	Append(wxID_ANY, _("Перейти к автору"), new FbMenuAuthors(m_id));
//	Append(wxID_ANY, _("Открыть серию"), NULL);
	AppendSeparator();

	if (iFolder == fbNO_FOLDER || iFolder) Append(ID_FAVORITES_ADD, _("Добавить в избранное"));
	Append(wxID_ANY, _("Добавить в папку"), new FbMenuFolders(iFolder));
	Append(wxID_ANY, _("Установить рейтинг"), new FbMenuRating);
	if (iFolder != fbNO_FOLDER) Append(ID_FAVORITES_DEL, _("Удалить закладку"));
	AppendSeparator();

	Append(ID_EDIT_COMMENTS, _("Комментарии"));
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
	if (sm_folders.IsEmpty()) LoadFolders();

	for (size_t i=0; i<sm_folders.Count(); i++) {
		int id = sm_folders[i].id;
		if (sm_folders[i].folder == folder) continue;
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
