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
	for (int i=5; i>0; i--) Append(ID_RATING_0 + i, GetRatingText(i));
	AppendSeparator();
	Append(ID_RATING_0, GetRatingText(0));
}
