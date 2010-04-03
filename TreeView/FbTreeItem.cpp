#include "FbTreeItem.h"

FbTreeItemId::FbTreeItemId(const FbTreeItemKey * key, const FbTreeItemKey * parent)
	: m_key(key ? key->Clone() : NULL), m_parent(parent ? parent->Clone() : NULL)
{
}

bool FbTreeItemId::operator ==(const FbTreeItemId &id) const
{
	if ((*m_key) == (*id.m_key)) {
		if (m_parent && id.m_parent) return (*m_parent) == (*id.m_parent);
		if (!m_parent && !id.m_parent) return true;
	}
	return false;
}
