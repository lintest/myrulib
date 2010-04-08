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

		FbTreeItemKey::KeyType GetType() const { return m_key ? m_key->GetType() : FbTreeItemKey::KT_NULL; };

		FbTreeItemKey * GetKey() const { return m_key; };

		FbTreeItemKey * GetParent() const { return m_parent; };

		bool IsOk() const { return m_key != NULL; }

	private:
		FbTreeItemKey * m_key;
		FbTreeItemKey * m_parent;
};

#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY(FbTreeItemId*, FbTreeItemIdArray);

WX_DECLARE_OBJARRAY(FbTreeItemKey*, FbTreeItemKeyArray);

template <FbTreeItemKey::KeyType type, typename T> 
class FbTreeItemKeyClass: public FbTreeItemKey                                  
{                                                                              
	public:                                                                    
		FbTreeItemKeyClass(T id): m_id(id) {}                                    
                                                                               
		virtual bool operator ==(const FbTreeItemKey &key) const               
			{ return key.GetType() == type && (*this) == (FbTreeItemKeyClass&)key; }     
                                                                               
		virtual bool operator ==(const FbTreeItemKeyClass &key) const                   
			{ return m_id == key.m_id; }                                       
                                                                               
		virtual KeyType GetType() const                                        
			{ return type; }                                                
                                                                               
		virtual FbTreeItemKey * Clone() const 
			{ return new FbTreeItemKeyClass(m_id); }
                                                                               
		T GetId() 
			{ return m_id; }                                      

		static FbTreeItemKeyClass * Key(const FbTreeItemId &id)
			{ return (id.GetType() == type) ? (FbTreeItemKeyClass*)id.GetKey() : NULL; }
                                                                         
	private:                                                                   
		T m_id;                                                         
};                                                                             

typedef FbTreeItemKeyClass<FbTreeItemKey::KT_LIST, size_t> FbTreeItemKeyList;

typedef FbTreeItemKeyClass<FbTreeItemKey::KT_ABCD, wxChar> FbTreeItemKeyAbcd;

typedef FbTreeItemKeyClass<FbTreeItemKey::KT_AUTH, int> FbTreeItemKeyAuth;

typedef FbTreeItemKeyClass<FbTreeItemKey::KT_SEQN, int> FbTreeItemKeySeqn;

typedef FbTreeItemKeyClass<FbTreeItemKey::KT_BOOK, int> FbTreeItemKeyBook;

#endif // __FBTREEITEMID_H__



