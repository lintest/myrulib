#include "FbTreeItem.h"

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(FbTreeItemIdArray);

WX_DEFINE_OBJARRAY(FbTreeItemKeyArray);

FbTreeItemId::FbTreeItemId(const FbTreeItemId &id)
	: m_key(id.m_key ? id.m_key->Clone() : NULL), m_parent(id.m_parent ? id.m_parent->Clone() : NULL)
{
}

FbTreeItemId & FbTreeItemId::operator =(const FbTreeItemId &id)
{
	wxDELETE(m_key);
	m_key = id.m_key ? id.m_key->Clone() : NULL;

	wxDELETE(m_parent);
	m_parent = id.m_parent ? id.m_parent->Clone() : NULL;

	return *this;
}


bool FbTreeItemId::operator ==(const FbTreeItemId &id) const
{
	if ((*m_key) == (*id.m_key)) {
		if (m_parent && id.m_parent) return (*m_parent) == (*id.m_parent);
		if (!m_parent && !id.m_parent) return true;
	}
	return false;
}


