#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"

//-----------------------------------------------------------------------------
//  FbMenuSort
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//  FbMenuRating
//-----------------------------------------------------------------------------

FbMenuRating::FbMenuRating()
{
	for (int i=5; i>0; i--) Append(ID_RATING_0 + i, GetRatingText(i));
	AppendSeparator();
	Append(ID_RATING_0, GetRatingText(0));
}

//-----------------------------------------------------------------------------
//  FbMenuRefs
//-----------------------------------------------------------------------------

wxMenuItem * FbMenuRefs::Create(wxMenu * menu)
{
	FbCommonDatabase database;
	if (!database.TableExists(wxT("tables"))) return NULL;

	wxMenu * submenu = NULL;

	wxString sql = wxT("SELECT title, id FROM tables ORDER BY 1");
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		if (!submenu) submenu = new FbMenuRefs();
		submenu->Append(wxID_ANY, res.GetString(0));
	}
	return submenu ? new wxMenuItem(menu, ID_FRAME_REFS, _("Directories"), wxEmptyString, wxITEM_NORMAL, submenu) : NULL;
}
