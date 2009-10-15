#include "FbFrameBaseMenu.h"
#include "FbConst.h"

FbFrameBaseMenu::FbFrameBaseMenu(): wxMenuBar()
{
	FbMenu * menu;

	menu = new FbMenu;
	menu->AppendImg(wxID_NEW,  _("Добавить файл\tCtrl+N"), wxART_NEW);
	menu->AppendImg(wxID_OPEN, _("Добавить директорию\tCtrl+O"), wxART_FOLDER_OPEN);
	menu->AppendSeparator();
	menu->AppendImg(wxID_SAVE, _("Экспорт книг\tCtrl+S"), wxART_FOLDER_OPEN);
	menu->AppendImg(wxID_EXIT, _("Выход\tAlt-F4"), wxART_QUIT);
	Append(menu, _("Файл"));

	menu = GetBookMenu();
	if (menu) Append(menu, _("Книги"));

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
	menu->Append(ID_FRAME_ARCH, _("Параметры"));
	Append(menu, _("Коллекция"));

	menu = new FbMenu;
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	Append(menu, _("Сервис"));

	menu = GetViewMenu();
	if (menu) Append(menu, _("Вид"));


	menu = new FbMenu;
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
	Append(menu, _("?"));
}

FbMenu * FbFrameBaseMenu::GetBookMenu()
{
	FbMenu * menu = new FbMenu;
	menu->Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu->Append(ID_UNSELECTALL, _("Отменить выделение"));
	return menu;
}

FbMenu * FbFrameBaseMenu::GetViewMenu()
{
	FbMenu * menu = new FbMenu;
	menu->AppendRadioItem(ID_MODE_TREE, _("&Иерархия авторов и серий"));
	menu->AppendRadioItem(ID_MODE_LIST, _("&Простой список"));
	menu->AppendSeparator();
	menu->AppendCheckItem(ID_FILTER_FB2, _("Фильтр: только fb2-файлы"));
	menu->AppendCheckItem(ID_FILTER_LIB, _("Фильтр: файлы Либрусек"));
	menu->AppendCheckItem(ID_FILTER_USR, _("Фильтр: файлы пользователя"));
	menu->AppendSeparator();
	menu->AppendRadioItem(ID_SPLIT_VERTICAL, _("&Просмотр справа"));
	menu->AppendRadioItem(ID_SPLIT_HORIZONTAL, _("&Просмтр снизу"));
	menu->AppendSeparator();
	menu->Append(ID_LOG_TEXTCTRL, _("Окно сообщений\tCtrl+Z"));
	return menu;
}
