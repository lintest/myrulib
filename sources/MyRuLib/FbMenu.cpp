#include "FbMenu.h"
#include "FbConst.h"

FbMenuSort::FbMenuSort()
{
	AppendRadioItem(wxID_VIEW_SORTNAME,  _("Title"));
	AppendRadioItem(ID_ORDER_AUTHOR, _("Author"));
	AppendRadioItem(ID_ORDER_RATING, _("Rating"));
	AppendRadioItem(ID_ORDER_LANG,   _("Language"));
	AppendRadioItem(wxID_VIEW_SORTTYPE,   _("Extension"));
	AppendRadioItem(wxID_VIEW_SORTSIZE,   _("Size"));
	AppendRadioItem(wxID_VIEW_SORTDATE,   _("Date"));
	AppendSeparator();
	AppendCheckItem(ID_DIRECTION, _("Reverse order"));
}

FbMenuRating::FbMenuRating()
{
	for (int i=5; i>0; i--) Append(ID_RATING_0 + i, GetRatingText(i));
	AppendSeparator();
	Append(ID_RATING_0, GetRatingText(0));
}
