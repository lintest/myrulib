#include "FbFilterObj.h"
#include "FbParams.h"

FbFilterObj::FbFilterObj(const FbFilterObj & obj)
	: m_enabled(obj.m_enabled), m_lib(obj.m_lib), m_usr(obj.m_usr), m_lang(obj.m_lang), m_type(obj.m_type)
{
}

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
	params.SetText(FB_FILTER_LANG, m_type);
	params.SetText(FB_FILTER_TYPE, m_type);
}

