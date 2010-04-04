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

class FbTreeItemKeyList: public FbTreeItemKey
{
	public:
		FbTreeItemKeyList(size_t id): m_id(id) {}

		virtual bool operator ==(const FbTreeItemKey &key) const
			{ return GetType() == key.GetType() && (*this) == (FbTreeItemKeyList&)key; }

		virtual bool operator ==(const FbTreeItemKeyList &key) const
			{ return m_id == key.m_id; }

		virtual KeyType GetType() const
			{ return KT_LIST; }

		virtual FbTreeItemKey * Clone() const { return new FbTreeItemKeyList(m_id); };

		size_t GetId() { return m_id; }

	private:
		size_t m_id;
};

class FbTreeItemKeyBook: public FbTreeItemKey
{
	public:
		FbTreeItemKeyBook(int id): m_id(id) {}

		virtual bool operator ==(const FbTreeItemKey &key) const
			{ return GetType() == key.GetType() && (*this) == (FbTreeItemKeyBook&)key; }

		virtual bool operator ==(const FbTreeItemKeyBook &key) const
			{ return m_id == key.m_id; }

		virtual KeyType GetType() const
			{ return KT_BOOK; }

		virtual FbTreeItemKey * Clone() const { return new FbTreeItemKeyBook(m_id); };

		size_t GetId() { return m_id; }

	private:
		int m_id;
};

#endif // __FBTREEITEMID_H__



