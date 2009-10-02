#include "FbFrameBase.h"
#include "FbConst.h"
#include "FbMenu.h"
#include "ExternalDlg.h"

BEGIN_EVENT_TABLE(FbFrameBase, wxAuiMDIChildFrame)
    EVT_MENU(wxID_SAVE, FbFrameBase::OnExternal)
	EVT_MENU(ID_SPLIT_HORIZONTAL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_SPLIT_VERTICAL, FbFrameBase::OnSubmenu)
	EVT_MENU(wxID_SELECTALL, FbFrameBase::OnSubmenu)
	EVT_MENU(ID_UNSELECTALL, FbFrameBase::OnSubmenu)
    EVT_UPDATE_UI(ID_SPLIT_HORIZONTAL, FbFrameBase::OnChangeViewUpdateUI)
    EVT_UPDATE_UI(ID_SPLIT_VERTICAL, FbFrameBase::OnChangeViewUpdateUI)
    EVT_UPDATE_UI(ID_MODE_LIST, FbFrameBase::OnChangeModeUpdateUI)
    EVT_UPDATE_UI(ID_MODE_TREE, FbFrameBase::OnChangeModeUpdateUI)
    EVT_COMMAND(ID_EMPTY_BOOKS, fbEVT_BOOK_ACTION, FbFrameBase::OnEmptyBooks)
    EVT_COMMAND(ID_APPEND_AUTHOR, fbEVT_BOOK_ACTION, FbFrameBase::OnAppendAuthor)
    EVT_COMMAND(ID_APPEND_SEQUENCE, fbEVT_BOOK_ACTION, FbFrameBase::OnAppendSequence)
    EVT_FB_BOOK(ID_APPEND_BOOK, FbFrameBase::OnAppendBook)
END_EVENT_TABLE()

FbFrameBase::FbFrameBase()
{
}

FbFrameBase::FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	Create(parent, id, title);
}

bool FbFrameBase::Create(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if (res) CreateControls();
	return res;
}

wxMenuBar * FbFrameBase::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	FbMenu * menu;

	menu = new FbMenu;
	menu->AppendImg(wxID_NEW, _("Добавить файл"), wxART_NEW);
	menu->AppendImg(wxID_OPEN, _("Добавить директорию"), wxART_FOLDER_OPEN);
	menu->AppendSeparator();
	menu->AppendImg(wxID_SAVE, _("Записать на устройство"), wxART_FILE_SAVE);
	menu->AppendSeparator();
	menu->AppendImg(wxID_EXIT, _("Выход\tAlt+F4"), wxART_QUIT);
	menuBar->Append(menu, _("&Файл"));

	menu = new FbMenu;
	menu->AppendImg(ID_MENU_SEARCH, _("Расширенный"), wxART_FIND);
	menu->AppendSeparator();
	menu->Append(ID_MENU_AUTHOR, _("по Автору"));
	menu->Append(ID_MENU_TITLE, _("по Заголовку"));
	menu->Append(ID_FRAME_GENRES, _("по Жанрам"));
	menu->AppendSeparator();
	menu->Append(ID_FRAME_FAVOUR, _("Избранное"));
	menuBar->Append(menu, _("&Поиск"));

	menu = new FbMenu;
	menu->Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu->Append(ID_UNSELECTALL, _("Отменить выделение"));
	menuBar->Append(menu, _("&Книги"));

	menu = new FbMenu;
	menu->Append(ID_MENU_DB_INFO, _("Информация о коллекции"));
	menu->Append(ID_MENU_VACUUM, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	menuBar->Append(menu, _("&Сервис"));

	menu = new FbMenu;
	menu->AppendRadioItem(ID_MODE_TREE, _("&Иерархия авторов и серий"));
	menu->AppendRadioItem(ID_MODE_LIST, _("&Простой список"));
	menu->AppendSeparator();
	menu->Append(ID_LOG_TEXTCTRL, _("Скрыть окно сообщений\tCtrl+Z"));
	menu->AppendSeparator();
	menu->AppendRadioItem(ID_SPLIT_VERTICAL, _("&Просмотр справа"));
	menu->AppendRadioItem(ID_SPLIT_HORIZONTAL, _("&Просмтр снизу"));
	menuBar->Append(menu, _("&Вид"));

	menu = new FbMenu;
	menu->Append(ID_OPEN_WEB, _("Официальный сайт"));
	menu->AppendImg(wxID_ABOUT, _("О программе…"), wxART_HELP_PAGE);
	menuBar->Append(menu, _("&?"));

	return menuBar;
}

void FbFrameBase::OnSubmenu(wxCommandEvent& event)
{
    wxPostEvent(&m_BooksPanel, event);
}

void FbFrameBase::OnChangeViewUpdateUI(wxUpdateUIEvent & event)
{
    if (event.GetId() == ID_SPLIT_HORIZONTAL && m_BooksPanel.GetSplitMode() == wxSPLIT_HORIZONTAL) event.Check(true);
    if (event.GetId() == ID_SPLIT_VERTICAL && m_BooksPanel.GetSplitMode() == wxSPLIT_VERTICAL) event.Check(true);
}

void FbFrameBase::OnChangeModeUpdateUI(wxUpdateUIEvent & event)
{
    if (event.GetId() == ID_MODE_LIST && m_BooksPanel.GetListMode() == FB2_MODE_LIST) event.Check(true);
    if (event.GetId() == ID_MODE_TREE && m_BooksPanel.GetListMode() == FB2_MODE_TREE) event.Check(true);
}

void FbFrameBase::OnExternal(wxCommandEvent& event)
{
    ExternalDlg::Execute(this, m_BooksPanel.m_BookList);
}

void FbFrameBase::OnEmptyBooks(wxCommandEvent& event)
{
	m_BooksPanel.EmptyBooks();
}

void FbFrameBase::OnAppendBook(FbBookEvent& event)
{
	m_BooksPanel.AppendBook( new BookTreeItemData(event.m_data), event.GetString() );
}

void FbFrameBase::OnAppendAuthor(wxCommandEvent& event)
{
	m_BooksPanel.AppendAuthor( event.GetString() );
}

void FbFrameBase::OnAppendSequence(wxCommandEvent& event)
{
	m_BooksPanel.AppendSequence( event.GetString() );
}

FbListMode FbFrameBase::GetListMode(FbParamKey key)
{
	return (bool)FbParams::GetValue(key) ? FB2_MODE_TREE : FB2_MODE_LIST;
}

void FbFrameBase::SetListMode(FbParamKey key, FbListMode mode)
{
	FbParams().SetValue(key, mode == FB2_MODE_TREE);
}

