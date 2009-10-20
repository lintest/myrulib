#include "FbBookMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"
#include "MyRuLibApp.h"

WX_DEFINE_OBJARRAY(FbMenuFolderArray);

WX_DEFINE_OBJARRAY(FbMenuAuthorArray);

FbMenuFolderArray FbBookMenu::sm_folders;

FbMenuAuthorArray FbBookMenu::sm_authors;

FbBookMenu::FbBookMenu(int id, int iFolder)
	: m_id(id)
{
    if (sm_folders.Count() == 0) LoadFolders();

	wxMenu * submenu = new wxMenu;
	for (size_t i=0; i<sm_folders.Count(); i++) {
		int id = sm_folders[i].id;
		if (sm_folders[i].folder == iFolder) continue;
		submenu->Append(id, sm_folders[i].name);
	}

	wxMenu * ratings = new wxMenu;
	ratings->Append(ID_RATING_5, wxT("* * * * *"));
	ratings->Append(ID_RATING_4, wxT("* * * *"));
	ratings->Append(ID_RATING_3, wxT("* * *"));
	ratings->Append(ID_RATING_2, wxT("* *"));
	ratings->Append(ID_RATING_1, wxT("*"));
    ratings->AppendSeparator();
	ratings->Append(ID_RATING_0, _("Очистить"));

	Append(ID_OPEN_BOOK, _("Открыть книгу\tEnter"));
	Append(ID_DOWNLOAD_BOOK, _("Скачать файл"));
    AppendSeparator();

	Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Отменить выделение"));
    AppendSeparator();

	Append(wxID_ANY, _("Перейти к автору"), CreateAuthorMenu());
    AppendSeparator();

    if (iFolder == fbNO_FOLDER) Append(ID_FAVORITES_ADD, _("Добавить в избранное"));
    else if (iFolder) Append(ID_FAVORITES_ADD, _("Добавить в избранное"));
	Append(wxID_ANY, _("Добавить в папку"), submenu);
	Append(wxID_ANY, _("Установить рейтинг"), ratings);
    if (iFolder != fbNO_FOLDER) Append(ID_FAVORITES_DEL, _("Удалить закладку"));
    AppendSeparator();

	Append(ID_EDIT_COMMENTS, _("Комментарии"));
}

void FbBookMenu::LoadFolders()
{
    wxString sql = wxT("SELECT id, value FROM folders ORDER BY value");
    wxSQLite3ResultSet result = wxGetApp().GetConfigDatabase().ExecuteQuery(sql);
    int id = ID_FAVORITES_ADD;
    while (result.NextRow()) {
        FbMenuFolderItem * item = new FbMenuFolderItem;
        item->id = ++id;
        item->folder = result.GetInt(0);
        item->name = result.GetString(1);
        sm_folders.Add(item);
    }
}

int FbBookMenu::GetFolder(const int id)
{
	for (size_t i=0; i<sm_folders.Count(); i++)
		if ( sm_folders[i].id == id ) return sm_folders[i].folder;
	return 0;
}

int FbBookMenu::GetAuthor(const int id)
{
	for (size_t i=0; i<sm_authors.Count(); i++)
		if ( sm_authors[i].id == id ) return sm_authors[i].author;
	return 0;
}

void FbBookMenu::ConnectFolders(wxWindow * frame, wxObjectEventFunction func)
{
	for (size_t i=0; i<sm_folders.Count(); i++)
		frame->Connect(sm_folders[i].id, wxEVT_COMMAND_MENU_SELECTED, func);
}

void FbBookMenu::ConnectAuthors(wxWindow * frame, wxObjectEventFunction func)
{
	for (size_t i=0; i<sm_authors.Count(); i++)
		frame->Connect(sm_authors[i].id, wxEVT_COMMAND_MENU_SELECTED, func);
}

wxMenu * FbBookMenu::CreateAuthorMenu()
{
    sm_authors.Empty();

    wxString sql = wxT("SELECT id, full_name FROM authors WHERE id IN (SELECT id_author FROM books WHERE id=?) ORDER BY search_name");
    FbCommonDatabase database;
    wxSQLite3Statement stmt = database.PrepareStatement(sql);
    stmt.Bind(1, m_id);
    wxSQLite3ResultSet result = stmt.ExecuteQuery();

	wxMenu * submenu = new wxMenu;
    int id = ID_FAVORITES_ADD + sm_folders.Count();
    while (result.NextRow()) {
    	FbMenuAuthorItem * item = new FbMenuAuthorItem;
    	item->id = ++id;
    	item->author = result.GetInt(0);
		submenu->Append(item->id, result.GetString(1));
    	sm_authors.Add(item);
    }
    return submenu;
}

