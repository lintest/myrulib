#include "FbBookMenu.h"
#include "FbConst.h"

FbBookMenu::FbBookMenu(int id)
	: m_id(id)
{
	Append(ID_OPEN_BOOK, _("Открыть книгу\tEnter"));
    AppendSeparator();
	Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Отменить выделение"));
    AppendSeparator();
    Append(ID_FAVORITES_ADD, _T("Добавить в избранное"));
}
