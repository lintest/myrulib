#include "FbMenu.h"
#include "FbConst.h"

FbMenuSort::FbMenuSort()
{
	AppendRadioItem(ID_ORDER_TITLE,  _("Заголовок"));
	AppendRadioItem(ID_ORDER_AUTHOR, _("Автор"));
	AppendRadioItem(ID_ORDER_RATING, _("Рейтинг"));
	AppendRadioItem(ID_ORDER_LANG,   _("Язык"));
	AppendRadioItem(ID_ORDER_TYPE,   _("Тип"));
	AppendRadioItem(ID_ORDER_SIZE,   _("Размер"));
	AppendRadioItem(ID_ORDER_DATE,   _("Дата"));
	AppendSeparator();
	AppendCheckItem(ID_DIRECTION, _("Обратный порядок"));
}

FbMenuRating::FbMenuRating()
{
	Append(ID_RATING_5, strRating[5]);
	Append(ID_RATING_4, strRating[4]);
	Append(ID_RATING_3, strRating[3]);
	Append(ID_RATING_2, strRating[2]);
	Append(ID_RATING_1, strRating[1]);
	AppendSeparator();
	Append(ID_RATING_0, strRating[0]);
}
