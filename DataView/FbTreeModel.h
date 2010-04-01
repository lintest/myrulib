#ifndef __FBTREEMODEL_H__
#define __FBTREEMODEL_H__

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/wxsqlite3.h>
#include <wx/arrimpl.cpp>
#include <wx/renderer.h>
#include "FbModelData.h"

class FbTreeModel;

class FbParentDataNode;

class FbTreeDataNode
{
	public:
        enum NodeType {
            NT_ROOT,
            NT_LETTER,
            NT_AUTHOR,
            NT_SEQUENCE,
            NT_BOOK,
        };
	public:
		FbTreeDataNode(bool checked = false): m_checked(FbTitleData::State(checked)) {};
        virtual NodeType GetType() = 0;
		virtual bool IsContainer() = 0;
		virtual FbParentDataNode * GetOwner() = 0;
		virtual wxDataViewItem GetParent() = 0;
        virtual void GetValue(const FbTreeModel &model, wxVariant &variant, unsigned int col) = 0;
        virtual bool SetValue(FbTreeModel &model, const wxVariant &variant, unsigned int col);
        virtual unsigned int GetChildren(const FbTreeModel &model, wxDataViewItemArray &children ) = 0;
        virtual void Check(FbTreeModel &model, bool checked = true) = 0;
    protected:
		FbTitleData::STATE m_checked;
		friend class FbParentDataNode;
};

WX_DECLARE_OBJARRAY(FbTreeDataNode*, FbTreeDataArray);

class FbParentTreeDataItem
{
    public:
        FbParentTreeDataItem(void * id, const wxString &text)
            : m_id(id), m_text(text) {};
        FbParentTreeDataItem(const FbParentTreeDataItem &item)
            : m_id(item.m_id), m_text(item.m_text) {};
        void * GetID() const { return m_id; };
        wxString GetText() const { return m_text; };
    private:
        void * m_id;
        wxString m_text;
};

WX_DECLARE_OBJARRAY(FbParentTreeDataItem, FbParentTreeDataArray);

class FbParentDataNode: public FbTreeDataNode
{
	public:
		FbParentDataNode(unsigned int count, bool checked = false): FbTreeDataNode(checked), m_count(count) {};
        virtual void Check(FbTreeModel &model, bool checked = true);
        virtual void CheckChildren(FbTreeModel &model);
		virtual bool IsContainer() { return true; };
        virtual void TestChildren(const FbTreeModel &model, NodeType type) = 0;
	protected:
        virtual wxString GetName(const FbTreeModel &model) = 0;
	protected:
		unsigned int m_count;
		FbTreeDataArray m_children;
};

class FbLetterDataNode: public FbParentDataNode
{
	public:
		FbLetterDataNode(wxChar letter, unsigned int count, bool checked = false): FbParentDataNode(count, checked), m_letter(letter) {};
        virtual NodeType GetType() { return NT_LETTER; };
		virtual FbParentDataNode * GetOwner() { return NULL; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(NULL); };
        virtual void GetValue(const FbTreeModel &model, wxVariant &variant, unsigned int col);
        virtual unsigned int GetChildren( const FbTreeModel &model, wxDataViewItemArray &children );
        virtual void TestChildren(const FbTreeModel &model, NodeType type);
	protected:
        virtual wxString GetName(const FbTreeModel &model) { return m_letter; };
	private:
		wxChar m_letter;
};

class FbAuthorDataNode: public FbParentDataNode
{
	public:
		FbAuthorDataNode(FbLetterDataNode * owner, bool checked = false): FbParentDataNode(0, checked), m_owner(owner), m_id(0) {};
        virtual NodeType GetType() { return NT_AUTHOR; };
		virtual FbParentDataNode * GetOwner() { return m_owner; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(m_owner); };
        virtual void GetValue(const FbTreeModel &model, wxVariant &variant, unsigned int col);
        virtual unsigned int GetChildren( const FbTreeModel &model, wxDataViewItemArray &children );
        virtual void TestChildren(const FbTreeModel &model, NodeType type);
        void SetId(int id) { m_id = id; };
        int GetId() { return m_id; };
	protected:
        virtual wxString GetName(const FbTreeModel &model);
        bool SeqExists(const FbTreeModel &model);
	private:
		FbLetterDataNode * m_owner;
		int m_id;
};

class FbSequenceDataNode: public FbParentDataNode
{
	public:
		FbSequenceDataNode(FbAuthorDataNode * owner, bool checked = false): FbParentDataNode(0, checked), m_owner(owner), m_id(0) {};
        virtual NodeType GetType() { return NT_SEQUENCE; };
		virtual FbParentDataNode * GetOwner() { return m_owner; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(m_owner); };
        virtual void GetValue(const FbTreeModel &model, wxVariant &variant, unsigned int col);
        virtual unsigned int GetChildren( const FbTreeModel &model, wxDataViewItemArray &children );
        virtual void TestChildren(const FbTreeModel &model, NodeType type);
        void SetId(int id, int count) { m_id = id; m_count = count; };
	protected:
        virtual wxString GetName(const FbTreeModel &model);
	private:
		FbAuthorDataNode * m_owner;
		int m_id;
};

class FbBookDataNode: public FbTreeDataNode
{
	public:
		FbBookDataNode(FbParentDataNode * owner, bool checked = false): FbTreeDataNode(checked), m_owner(owner), m_id(0) {};
        virtual NodeType GetType() { return NT_BOOK; };
		virtual bool IsContainer() { return false; };
		virtual FbParentDataNode * GetOwner() { return m_owner; };
		virtual wxDataViewItem GetParent() { return wxDataViewItem(m_owner); };
        virtual void GetValue(const FbTreeModel &model, wxVariant &variant, unsigned int col);
        virtual unsigned int GetChildren( const FbTreeModel &model, wxDataViewItemArray &children ) { return 0; };
        virtual void Check(FbTreeModel &model, bool checked = true);
        void SetId(int id) { m_id = id; };
	private:
        wxString GetName(const FbTreeModel &model);
	private:
		FbParentDataNode * m_owner;
		int m_id;
};

class FbTreeModelData
{
    public:
        FbTreeModelData(unsigned int id = 0): m_rowid(id), m_bookid(0), m_filesize(0) {};
        FbTreeModelData(wxSQLite3ResultSet &result);
        FbTreeModelData(const FbTreeModelData &data);
        wxString GetValue(unsigned int col);
		wxString GetAuthors(wxSQLite3Database &database);
        unsigned int Id() { return m_rowid; };
    private:
		wxString Format(const int number);
    private:
        int m_rowid;
        int m_bookid;
        wxString m_title;
        wxString m_authors;
        int m_filesize;
};

class FbTreeModel: public wxDataViewModel
{
    public:
        FbTreeModel(const wxString &filename);
        virtual ~FbTreeModel();
        virtual unsigned int GetColumnCount() const { return fbCOL_MAX; };
        virtual wxString GetColumnType( unsigned int col ) const { return wxT("string"); };
		virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
		virtual bool IsContainer( const wxDataViewItem &item ) const;
        virtual void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const;
        virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);
        virtual bool GetAttr(const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr) const;
        virtual unsigned int GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const;
        virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const { return true; };
        virtual void Resort() {};
        wxSQLite3Database * GetDatabase() const { return m_database; };

    public:
		bool FindItemText(void * id, wxString &text) const;
        const wxString & PushItemText(void * id, const wxString &text) const;

    protected:
        static FbTreeDataNode * Node(const wxDataViewItem &item) { return (FbTreeDataNode*)(item.GetID());};

    protected:
        wxSQLite3Database * m_database;
		FbParentTreeDataArray * m_parents;
		FbTreeDataArray m_children;
};

class FbFullTreeModel: public FbTreeModel
{
    public:
        FbFullTreeModel(const wxString &filename);
};

#endif // __FBTREEMODEL_H__

