#include "FbMenu.h"
#include "FbConst.h"

FbMenuSort::FbMenuSort()
{
	AppendRadioItem(ID_ORDER_TITLE,  _("Title"));
	AppendRadioItem(ID_ORDER_AUTHOR, _("Author"));
	AppendRadioItem(ID_ORDER_RATING, _("Rating"));
	AppendRadioItem(ID_ORDER_LANG,   _("Language"));
	AppendRadioItem(ID_ORDER_TYPE,   _("Extension"));
	AppendRadioItem(ID_ORDER_SIZE,   _("Size"));
	AppendRadioItem(ID_ORDER_DATE,   _("Date"));
	AppendSeparator();
	AppendCheckItem(ID_DIRECTION, _("Reverse order"));
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
