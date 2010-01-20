#include "FbFilterObj.h"
#include "FbParams.h"

FbFilterObj::FbFilterObj() :
	m_enabled(FbParams::GetValue(FB_USE_FILTER)),
	m_lib(FbParams::GetValue(FB_FILTER_LIB)),
	m_usr(FbParams::GetValue(FB_FILTER_USR)),
	m_lang(FbParams::GetText(FB_FILTER_LANG)),
	m_type(FbParams::GetText(FB_FILTER_TYPE))
{
}

void FbFilterObj::Save()
{
	FbParams params;
	params.SetValue(FB_USE_FILTER, m_enabled);
	params.SetValue(FB_FILTER_LIB, m_lib);
	params.SetValue(FB_FILTER_USR, m_usr);
	params.SetText(FB_FILTER_LANG, m_lang);
	params.SetText(FB_FILTER_TYPE, m_type);
}

wxString FbFilterObj::GetSQL()
{
	if (!m_enabled) return wxEmptyString;

	const wxString addin = wxT(" AND books.");

	wxString sql;
	if (m_lib && !m_usr) sql += addin + wxT("id>0");
	if (!m_lib && m_usr) sql += addin + wxT("id<0");
	if (!m_lang.IsEmpty()) sql += addin + wxT("lang in (") + m_lang + wxT(")");
	if (!m_type.IsEmpty()) sql += addin + wxT("file_type in (") + m_type + wxT(")");

	return sql;
}

