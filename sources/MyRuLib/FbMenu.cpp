#include "FbMenu.h"
#include "FbConst.h"
#include "FbDatabase.h"

//-----------------------------------------------------------------------------
//  FbMenu
//-----------------------------------------------------------------------------

wxMenuItem * FbMenu::AppendSub(Type type, int code, const wxString& name, const wxString& help, wxItemKind kind)
{
	wxMenuItem * item = new FbMenuItem(this, type, code, name, help, kind);
	return Append(item);
}

//-----------------------------------------------------------------------------
//  FbMenuItem
//-----------------------------------------------------------------------------

FbMenuHash FbMenuItem::m_hash;

wxWindowID FbMenuItem::NewId(FbMenu::Type type, int code)
{
	wxWindowID id = wxNewId();
	FbMenuData & data = m_hash[id];
	data.code = code;
	data.type = type;
	return id;
}

bool FbMenuItem::Get(wxWindowID id, FbMenu::Type & type, int & code)
{
	bool ok = m_hash.count(id);
	if (ok) {
		FbMenuData & data = m_hash[id];
		type = data.type;
		code = data.code;
	}
	return ok;
}

FbMenuItem::FbMenuItem(wxMenu * menu, FbMenu::Type type, int code, const wxString& name, const wxString& help, wxItemKind kind, wxMenu * submenu )
	: wxMenuItem(menu, NewId(type, code), name, help, kind, submenu)
{
}

FbMenuItem::~FbMenuItem()
{
	m_hash.erase(GetId());
}

//-----------------------------------------------------------------------------
//  FbMenuSort
//-----------------------------------------------------------------------------

FbMenuSort::FbMenuSort()
{
	AppendRadioItem(wxID_VIEW_SORTNAME, _("Title"));
	AppendRadioItem(ID_ORDER_AUTHOR,    _("Author"));
	AppendRadioItem(ID_ORDER_RATING,    _("Rating"));
	AppendRadioItem(ID_ORDER_LANG,      _("Language"));
	AppendRadioItem(wxID_VIEW_SORTTYPE, _("Extension"));
	AppendRadioItem(wxID_VIEW_SORTSIZE, _("Size"));
	AppendRadioItem(wxID_VIEW_SORTDATE, _("Date"));
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

	FbMenu * submenu = NULL;

	wxString sql = wxT("SELECT id, title FROM tables ORDER BY 2");
	wxSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		if (!submenu) submenu = new FbMenuRefs();
		submenu->AppendSub(FbMenu::CLSS, res.GetInt(0), res.GetString(1));
	}
	return submenu ? new wxMenuItem(menu, ID_FRAME_CLSS, _("Classifiers"), wxEmptyString, wxITEM_NORMAL, submenu) : NULL;
}
