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
		FbTreeItemId(const FbTreeItemKey * key, const FbTreeItemKey * parent = NULL)
			: m_key(key->Clone()), m_parent(parent ? parent->Clone() : NULL) {}

		virtual ~FbTreeItemId()
			{ wxDELETE(m_key); wxDELETE(m_parent); }

		bool operator ==(const FbTreeItemId &id) const;

	private:
		FbTreeItemKey * m_key;
		FbTreeItemKey * m_parent;
};

class FbTreeItemKeyNull: public FbTreeItemKey
{
	public:
		FbTreeItemKeyNull() {}

		virtual bool operator ==(const FbTreeItemKey &key) const
			{ return GetType() == key.GetType() && (*this) == (FbTreeItemKeyNull&)key; }

		virtual bool operator ==(const FbTreeItemKeyNull &key) const
			{ return true; }

		virtual KeyType GetType() const
			{ return KT_NULL; }
};

class FbTreeItemKeyList: public FbTreeItemKey
{
	public:
		FbTreeItemKeyList(int id): m_id(id) {}

		virtual bool operator ==(const FbTreeItemKey &key) const
			{ return GetType() == key.GetType() && (*this) == (FbTreeItemKeyList&)key; }

		virtual bool operator ==(const FbTreeItemKeyList &key) const
			{ return m_id == key.m_id; }

		virtual KeyType GetType() const
			{ return KT_LIST; }

	private:
		int m_id;
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

	private:
		int m_id;
};

#endif // __FBTREEITEMID_H__
