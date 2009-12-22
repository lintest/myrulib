#include "FbMainMenu.h"
#include "FbConst.h"

FbMenuBar::MenuFrame::MenuFrame()
{
	AppendImg(ID_MENU_SEARCH, _("Поиск"), wxART_FIND);
	AppendSeparator();
	Append(ID_FRAME_AUTHOR, _("Авторы"));
	Append(ID_FRAME_GENRES, _("Жанры"));
	Append(ID_FRAME_SEQUEN, _("Серии"));
	Append(ID_FRAME_DATE,   _("Календарь"));
	Append(ID_FRAME_ARCH,   _("Архив"));
	AppendSeparator();
	Append(ID_FRAME_FOLDER, _("Мои папки"));
	Append(ID_FRAME_DOWNLD, _("Загрузки"));
}

FbMenuBar::MenuLib::MenuLib()
{
	Append(ID_MENU_DB_OPEN, _("Открыть (создать) коллекцию"));
	Append(wxID_ANY, _("Открыть повторно"), new MenuRecent());
	AppendSeparator();
	Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	Append(ID_MENU_VACUUM,  _("Реструктуризация БД"));
	AppendSeparator();
	Append(ID_MENU_CONFIG, _("Параметры"));
}

FbMenuBar::MenuSetup::MenuSetup()
{
	Append(wxID_PREFERENCES, _("Настройки"));
}

FbMenuBar::MenuHelp::MenuHelp()
{
	Append(ID_OPEN_WEB, _("Официальный сайт"));
	AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
}

FbMainMenu::FbMainMenu(): FbMenuBar()
{
	Append(new MenuFile,  _("Файл"));
	Append(new MenuLib,   _("Библиотека"));
	Append(new MenuFrame, _("Картотека"));
	Append(new MenuView,  _("Вид"));
	Append(new MenuSetup, _("Сервис"));
	Append(new MenuHelp,  _("?"));
}

FbMainMenu::MenuFile::MenuFile()
{
	AppendImg(wxID_NEW,  _("Добавить файл\tCtrl+N"), wxART_NEW);
	AppendImg(wxID_OPEN, _("Добавить директорию\tCtrl+O"), wxART_FOLDER_OPEN);
	AppendSeparator();
	AppendImg(wxID_EXIT, _("Выход\tAlt-F4"), wxART_QUIT);
}

FbMainMenu::MenuView::MenuView()
{
	Append(ID_FULLSCREEN, _("Полноэкранный режим\tF11"));
	Append(ID_LOG_TEXTCTRL, _("Окно сообщений\tF12"));
}

FbFrameMenu::MenuFile::MenuFile()
{
	AppendImg(wxID_NEW,  _("Добавить файл\tCtrl+N"), wxART_NEW);
	AppendImg(wxID_OPEN, _("Добавить директорию\tCtrl+O"), wxART_FOLDER_OPEN);
	AppendSeparator();
	AppendImg(wxID_SAVE, _("Экспорт книг\tCtrl+S"), wxART_FILE_SAVE);
	AppendImg(wxID_EXIT, _("Выход\tAlt-F4"), wxART_QUIT);
}

FbFrameMenu::MenuBook::MenuBook()
{
	Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Отменить выделение"));
	AppendSeparator();
	Append(ID_EDIT_COMMENTS, _("Комментарии"));
}

FbFrameMenu::MenuView::MenuView()
{
	AppendRadioItem(ID_MODE_TREE, _("&Иерархия авторов и серий"));
	AppendRadioItem(ID_MODE_LIST, _("&Простой список"));
	AppendSeparator();
	Append(ID_ORDER_MENU, _("Сортировка"), new FbMenuSort());
	Append(wxID_ANY, _("Фильтр"), new FbMenuFilter());
	AppendSeparator();
	AppendRadioItem(ID_SPLIT_VERTICAL, _("&Просмотр справа"));
	AppendRadioItem(ID_SPLIT_HORIZONTAL, _("&Просмотр снизу"));
	AppendSeparator();
	Append(ID_FULLSCREEN, _("Полноэкранный режим\tF11"));
	Append(ID_LOG_TEXTCTRL, _("Окно сообщений\tF12"));
}

FbMenuBar::MenuRecent::MenuRecent()
{
	Append(ID_RECENT_1);
	Append(ID_RECENT_2);
	Append(ID_RECENT_3);
	Append(ID_RECENT_4);
	Append(ID_RECENT_5);
}

FbMenuBar::MenuWindow::MenuWindow()
{
	Append(ID_WINDOW_CLOSE,    _("Закрыть окно"));
	Append(ID_WINDOW_CLOSEALL, _("Закрыть все"));
	AppendSeparator();
	Append(ID_WINDOW_NEXT,     _("Следующее"));
	Append(ID_WINDOW_PREV,     _("Предыдущее"));
}
