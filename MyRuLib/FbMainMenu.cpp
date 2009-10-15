#include "FbMainMenu.h"
#include "FbConst.h"

FbMainMenu::FbMainMenu(): wxMenuBar()
{
	FbMenu * menu;

	menu = new FbMenu;
	menu->AppendImg(wxID_NEW, _("Добавить файл\tCtrl+N"), wxART_NEW);
	menu->AppendImg(wxID_OPEN, _("Добавить директорию\tCtrl+O"), wxART_FOLDER_OPEN);
	menu->AppendSeparator();
	menu->AppendImg(wxID_EXIT, _("Выход\tAlt-F4"), wxART_QUIT);
	Append(menu, _("Файл"));

	menu = new FbMenu;
	menu->AppendImg(ID_MENU_SEARCH, _("Поиск"), wxART_FIND);
	menu->AppendSeparator();
	menu->Append(ID_MENU_AUTHOR, _("Авторы"));
	menu->Append(ID_FRAME_GENRES, _("Жанры"));
	menu->Append(ID_FRAME_SEQ, _("Серии"), wxART_FIND);
	menu->Append(ID_FRAME_DATE, _("Календарь"), wxART_FIND);
	menu->Append(ID_FRAME_ARCH, _("Архив"), wxART_FIND);
	menu->AppendSeparator();
	menu->Append(ID_FRAME_FAVOUR, _("Избранное"));
	Append(menu, _("Картотека"));

	menu = new FbMenu;
	menu->Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	menu->Append(ID_MENU_VACUUM, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(ID_MENU_VACUUM, _("Параметры"));
	Append(menu, _("Коллекция"));

	menu = new FbMenu;
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	Append(menu, _("Сервис"));

	menu = new FbMenu;
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
	Append(menu, _("?"));
}
