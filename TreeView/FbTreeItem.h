#ifndef __FBTREEITEMID_H__
#define __FBTREEITEMID_H__

#include <wx/defs.h>

class FbTreeItemKey
{
	public:
		enum KeyType {
			KT_NULL,
			KT_LIST,
			KT_BOOK,
			KT_SEQN,
			KT_AUTH,
			KT_ABCD,
		};

	public:
		virtual bool operator ==(const FbTreeItemKey &key) const = 0;
		virtual FbTreeItemKey * Clone() const = 0;
		virtual KeyType GetType() const = 0;
};

class FbTreeItemId
{
	public:
		FbTreeItemId()
			: m_key(NULL), m_parent(NULL) {}

		FbTreeItemId(const FbTreeItemKey & key)
			: m_key(key.Clone()), m_parent(NULL) {}

		FbTreeItemId(const FbTreeItemKey & key, const FbTreeItemKey & parent);

		FbTreeItemId(const FbTreeItemId &id);

		virtual ~FbTreeItemId()
			{ wxDELETE(m_key); wxDELETE(m_parent); }

		FbTreeItemId & operator =(const FbTreeItemId &id);

		bool operator ==(const FbTreeItemId &id) const;

		FbTreeItemKey::KeyType GetKeyType() const { return m_key ? m_key->GetType() : FbTreeItemKey::KT_NULL; };

		FbTreeItemKey * GetKey() const { return m_key; };

		FbTreeItemKey * GetParent() const { return m_parent; };

		bool IsOk() const { return m_key; }

	private:
		FbTreeItemKey * m_key;
		FbTreeItemKey * m_parent;
};

#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY(FbTreeItemId*, FbTreeItemIdArray);

WX_DECLARE_OBJARRAY(FbTreeItemKey*, FbTreeItemKeyArray);

#define FB_DECLARE_ITEMKEY(ClassName, ClassId, SubClass)                       \
class ClassName: public FbTreeItemKey                                          \
{                                                                              \
	public:                                                                    \
		ClassName(SubClass id): m_id(id) {}                                    \
                                                                               \
		virtual bool operator ==(const FbTreeItemKey &key) const               \
			{ return GetType() == ClassId && (*this) == (ClassName&)key; }     \
                                                                               \
		virtual bool operator ==(const ClassName &key) const                   \
			{ return m_id == key.m_id; }                                       \
                                                                               \
		virtual KeyType GetType() const                                        \
			{ return ClassId; }                                                \
                                                                               \
		virtual FbTreeItemKey * Clone() const { return new ClassName(m_id); }; \
                                                                               \
		SubClass GetId() { return m_id; }                                      \
                                                                               \
	private:                                                                   \
		SubClass m_id;                                                         \
};                                                                             \

FB_DECLARE_ITEMKEY(FbTreeItemKeyList, KT_LIST, size_t)

FB_DECLARE_ITEMKEY(FbTreeItemKeyAbcd, KT_ABCD, wxChar)

FB_DECLARE_ITEMKEY(FbTreeItemKeyAuth, KT_AUTH, int)

FB_DECLARE_ITEMKEY(FbTreeItemKeySeqn, KT_SEQN, int)

FB_DECLARE_ITEMKEY(FbTreeItemKeyBook, KT_BOOK, int)

#endif // __FBTREEITEMID_H__



