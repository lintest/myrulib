#include "FbMainMenu.h"
#include "FbConst.h"

FbMainMenu::FbMainMenu(): wxMenuBar()
{
	FbMenu * menu;

	menu = new FbMenu;
	menu->AppendImg(wxID_NEW, _("Добавить файл"), wxART_NEW);
	menu->AppendImg(wxID_OPEN, _("Добавить директорию"), wxART_FOLDER_OPEN);
	menu->AppendSeparator();
	menu->AppendImg(wxID_EXIT, _("Выход\tAlt+F4"), wxART_QUIT);
	Append(menu, _("&Файл"));

	menu = new FbMenu;
	menu->AppendImg(ID_MENU_SEARCH, _("Расширенный"), wxART_FIND);
	menu->AppendSeparator();
	menu->Append(ID_MENU_AUTHOR, _("по Автору"));
	menu->Append(ID_MENU_TITLE, _("по Заголовку"));
	menu->Append(ID_FRAME_GENRES, _("по Жанрам"));
	menu->AppendSeparator();
	menu->Append(ID_FRAME_FAVOUR, _("Избранное"));
	Append(menu, _("&Поиск"));

	menu = GetBookMenu();
	if (menu) Append(menu, _("&Книги"));

	menu = new FbMenu;
	menu->Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	menu->Append(ID_MENU_VACUUM, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	Append(menu, _("&Сервис"));

	menu = GetViewMenu();
	if (menu) Append(menu, _("&Вид"));


	menu = new FbMenu;
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
	Append(menu, _("&?"));
}
