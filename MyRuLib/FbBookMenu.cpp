#include "FbBookMenu.h"
#include "FbConst.h"

FbBookMenu::FbBookMenu(int id, bool bFavorites)
	: m_id(id)
{
	Append(ID_OPEN_BOOK, _("Открыть книгу\tEnter"));
    AppendSeparator();
	Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	Append(ID_UNSELECTALL, _("Отменить выделение"));
    AppendSeparator();
    if (bFavorites) {
        Append(ID_FAVORITES_DEL, _T("Удалить закладку"));
    } else {
        Append(ID_FAVORITES_ADD, _T("Добавить в избранное"));
    }
}
