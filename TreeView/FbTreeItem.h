#ifndef __FBTREEITEMID_H__
#define __FBTREEITEMID_H__

#include <wx/defs.h>

class FbTreeItemData
{
	public:
		enum Type {
			TY_NULL,
			TY_ROW,
			TY_BOOK,
		};

	public:
		virtual bool operator ==(const FbTreeItemData &data) const = 0;
		virtual FbTreeItemData * Clone() const = 0;
		virtual Type GetType() const = 0;
};

class FbTreeItemId
{
	public:
		FbTreeItemId(const FbTreeItemData &data)
			: m_data(data.Clone()) {}

		virtual ~FbTreeItemId()
			{ wxDELETE(m_data); }

		bool operator ==(const FbTreeItemId &id) const
			{ return (*m_data) == (*id.m_data); }

	private:
		FbTreeItemData * m_data;
};

class FbTreeItemDataNull: public FbTreeItemData
{
	public:
		FbTreeItemDataNull() {}

		virtual bool operator ==(const FbTreeItemData &data) const
			{ return GetType() == data.GetType() && (*this) == (FbTreeItemDataNull&)data; }

		virtual bool operator ==(const FbTreeItemDataNull &data) const
			{ return true; }

		virtual Type GetType() const
			{ return TY_NULL; }
};

class FbTreeItemDataRow: public FbTreeItemData
{
	public:
		FbTreeItemDataRow(int id): m_id(id) {}

		virtual bool operator ==(const FbTreeItemData &data) const
			{ return GetType() == data.GetType() && (*this) == (FbTreeItemDataRow&)data; }

		virtual bool operator ==(const FbTreeItemDataRow &data) const
			{ return m_id == data.m_id; }

		virtual Type GetType() const
			{ return TY_ROW; }

	private:
		int m_id;
};

class FbTreeItemDataBook: public FbTreeItemData
{
	public:
		FbTreeItemDataBook(int id): m_id(id) {}

		virtual bool operator ==(const FbTreeItemData &data) const
			{ return GetType() == data.GetType() && (*this) == (FbTreeItemDataBook&)data; }

		virtual bool operator ==(const FbTreeItemDataBook &data) const
			{ return m_id == data.m_id; }

		virtual Type GetType() const
			{ return TY_BOOK; }

	private:
		int m_id;
};

#endif // __FBTREEITEMID_H__
